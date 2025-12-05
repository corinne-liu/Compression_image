#include "bitstream.h"

size_t pushbits(BitStream* courant, unsigned char source, size_t nombre_bit) {
    size_t bits_ecrits = 0;

    while (nombre_bit > 0) {
        size_t bits_restant;
        if (nombre_bit <= courant->capacite) {
            bits_restant = nombre_bit;
        }
        else {
            bits_restant = courant->capacite;
        }
        unsigned char bits_alignes = source >> (nombre_bit - bits_restant);
        bits_alignes &= (1 << bits_restant) - 1;
        *courant->buffer |= bits_alignes << (courant->capacite - bits_restant);
        courant->capacite -= bits_restant;
        nombre_bit -= bits_restant;
        bits_ecrits += bits_restant;

        if (courant->capacite == 0) {
            courant->buffer++;
            *courant->buffer = 0;
            courant->capacite = CHAR_BIT;
        }
    }
    return bits_ecrits;
}

size_t pullbits(BitStream* courant, unsigned char* destination, size_t nombre_bit) {
    size_t bits_lus = 0;
    *destination = 0;

    while (nombre_bit > 0) {
        size_t bits_restant;
        if (nombre_bit <= courant->capacite) {
            bits_restant = nombre_bit;
        }
        else {
            bits_restant = courant->capacite;
        }

        unsigned char bits_extraits = (*courant->buffer >> (courant->capacite - bits_restant)) & ((1 << bits_restant) - 1);
        *destination = (*destination << bits_restant) | bits_extraits;
        courant->capacite -= bits_restant;
        nombre_bit -= bits_restant;
        bits_lus += bits_restant;

        if (courant->capacite == 0) {
            courant->buffer++;
            courant->capacite = CHAR_BIT;
        }
    }
    return bits_lus;
}