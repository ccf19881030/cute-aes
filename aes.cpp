#include "aes.h"

CuteAES::CuteAES(aes_mode_t _aes_mode, crypt_mode_t _crypt_mode)
{
    // for now only AES128 with EBC is implemented, so others are forbidden
    if (aes_mode != AES_128_MODE and crypt_mode != ECB_MODE)
        throw("Сan only use AES128 with EBC");

    this->aes_mode = _aes_mode;
    this->crypt_mode = _crypt_mode;

    switch (aes_mode) {
        case (AES_128_MODE):
            this->aes_info.nk = 4;
            this->aes_info.nr = 10;
            this->aes_info.key_length = 16;
            break;

        case (AES_192_MODE):
            this->aes_info.nk = 6;
            this->aes_info.nr = 12;
            this->aes_info.key_length = 24;
            break;

        case (AES_256_MODE):
            this->aes_info.nk = 8;
            this->aes_info.nr = 14;
            this->aes_info.key_length = 32;
            break;

        default:
            throw("Unknown AES mode!");
    }
}

QByteArray CuteAES::Encrypt(QByteArray &text, QByteArray &key)
{
    CuteAES aes(AES_128_MODE, ECB_MODE);
    return aes.encrypt(text, key, nullptr);
}

QByteArray CuteAES::Decrypt(QByteArray &text, QByteArray &key)
{
    CuteAES aes(AES_128_MODE, ECB_MODE);
    return aes.decrypt(text, key, nullptr);
}

QByteArray CuteAES::encrypt(QByteArray &text, QByteArray &key, const QByteArray &iv)
{
    if (text == nullptr or key == nullptr)
        return nullptr;

    QByteArray ret;
    QByteArray expanded_key = expandKey(key);
    alignText(text);

    switch (crypt_mode) {
        case (ECB_MODE):
            for (int i = 0; i < text.size(); i += blocklen)
                ret.append(cipher(expanded_key, text.mid(i, blocklen)));

            break;

        default:
            return nullptr;
    }

    return ret;
}

QByteArray CuteAES::decrypt(QByteArray &text, QByteArray &key, const QByteArray &iv)
{
    if (text == nullptr or key == nullptr)
        return nullptr;

    QByteArray ret;
    QByteArray expanded_key = expandKey(key);

    switch (crypt_mode) {
        case (ECB_MODE):
            for (int i = 0; i < text.size(); i += blocklen)
                ret.append(cipher(expanded_key, text.mid(i, blocklen)));

            break;

        default:
            return nullptr;
    }

    return ret;
}

QByteArray CuteAES::expandKey(QByteArray &key)
{
    quint8 temp[4];
    QByteArray roundKey(key);

    for (int i = aes_info.nk; i < nb * (aes_info.nr + 1); i++) {
        for (int j = 0; j < 4; j++) {
            temp[j] = static_cast<quint8>(roundKey[(i - 1) * 4 + j]);
        }

        if (i % aes_info.nk) {
            quint8 local = temp[0];

            // Rotate word
            temp[0] = temp[1];
            temp[1] = temp[2];
            temp[2] = temp[3];
            temp[3] = local;

            // Substitute word
            for (int j = 0; j < 4; j++) {
                temp[j] = getSboxValue(temp[j]);
            }

            temp[0] ^= Rcon[i / aes_info.nk];
        }

        for (int j = 0; j < 4; j++) {
            roundKey.append(i * 4 + j, static_cast<qint8>(
                roundKey[(i - aes_info.nk) * 4 + j] ^ temp[j])
            );
        }
    }

    return roundKey;
}

void CuteAES::alignText(QByteArray &text)
{
    int size = (blocklen - text.size() % blocklen) % blocklen;
    text.append(QByteArray(size, 0));
}

QByteArray CuteAES::cipher(QByteArray &ext_key, const QByteArray &input)
{
    QByteArray output(input);

    addRoundKey(&output, 0, ext_key);

    for (quint8 round = 1; round < aes_info.nr; round++) {
        subBytes(&output);
        shiftRows(&output);
        mixColumns(&output);
        addRoundKey(&output, round, ext_key);
    }

    subBytes(&output);
    shiftRows(&output);
    addRoundKey(&output, aes_info.nr, ext_key);

    return output;
}

QByteArray CuteAES::decipher(QByteArray &ext_key, const QByteArray &in)
{
    return nullptr;
}

void CuteAES::addRoundKey(QByteArray *state, quint8 round, QByteArray ext_key)
{
    QByteArray::iterator iter = state->begin();

    for (int i = 0; i < 16; i++) {
        iter[i] = static_cast<qint8>(iter[i]) ^ static_cast<qint8>(ext_key[
            round * nb * 4 + (i / 4) * nb + (i % 4)
        ]);
    }
}

void CuteAES::subBytes(QByteArray *state)
{
    QByteArray::iterator iter = state->begin();

    for (int i = 0; i < 16; i++) {
        iter[i] = static_cast<qint8>(getSboxValue(
            static_cast<quint8>(iter[i])
        ));
    }
}

void CuteAES::shiftRows(QByteArray *state)
{
    return;
}

void CuteAES::mixColumns(QByteArray *state)
{
    return;
}
