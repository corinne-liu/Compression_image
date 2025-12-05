#ifndef BITSTREAM_H
    #define BITSTREAM_H

    #include <stdio.h>
    #include <limits.h>

    /**
     * @struct BitStream
     * @brief Représente un flux de bits pour l'écriture et la lecture de bits.
     *
     * Cette structure contient un buffer qui stocke les bits à lire ou à écrire ainsi que la capacité restante dans l'octet courant.
     */
    typedef struct {
        unsigned char* buffer;    /**< Pointeur vers l'octet courant du flux de bits. */
        unsigned long capacite;   /**<Bits restants dans l'octet courant. */
    } BitStream;

    /**
     * @brief Écrit un nombre donné de bits dans le flux de bits.
     *
     * @param courant Pointeur vers la structure BitStream où les bits seront écrits.
     * @param source L'octet contenant les bits à écrire.
     * @param nombre_bit Le nombre de bits à écrire.
     * @return Le nombre de bits effectivement écrits.
     */
    size_t pushbits(BitStream* stream, unsigned char src, size_t nbit);
    /**
     * @brief Lit un nombre donné de bits depuis le flux de bits.
     *
     * @param courant Pointeur vers la structure BitStream à partir de laquelle les bits seront lus.
     * @param destination Pointeur vers l'octet où les bits seront stockés.
     * @param nombre_bit Le nombre de bits à lire.
     * @return Le nombre de bits effectivement lus.
     */
    size_t pullbits(BitStream* curr, unsigned char* dest, size_t nbit);

#endif