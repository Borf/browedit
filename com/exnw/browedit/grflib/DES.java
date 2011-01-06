package com.exnw.browedit.grflib;

class DES {
    private static byte[][] BitSwapTable = new byte[][]{
                                                        // BitSwapTable 1
                                                        new byte[]{
                                                            58, 50, 42, 34, 26, 18, 10,  2, 60, 52, 44, 36, 28, 20, 12,  4,
                                                            62, 54, 46, 38, 30, 22, 14,  6, 64, 56, 48, 40, 32, 24, 16,  8,
                                                            57, 49, 41, 33, 25, 17,  9,  1, 59, 51, 43, 35, 27, 19, 11,  3,
                                                            61, 53, 45, 37, 29, 21, 13,  5, 63, 55, 47, 39, 31, 23, 15,  7
                                                        },
                                                        // BitSwapTable 2
                                                        new byte[]{
                                                            40,  8, 48, 16, 56, 24, 64, 32, 39,  7, 47, 15, 55, 23, 63, 31,
                                                            38,  6, 46, 14, 54, 22, 62, 30, 37,  5, 45, 13, 53, 21, 61, 29,
                                                            36,  4, 44, 12, 52, 20, 60, 28, 35,  3, 43, 11, 51, 19, 59, 27,
                                                            34,  2, 42, 10, 50, 18, 58, 26, 33,  1, 41,  9, 49, 17, 57, 25
                                                        },
                                                        // BitSwapTable 3
                                                        new byte[]{
                                                            16,  7, 20, 21, 29, 12, 28, 17,  1, 15, 23, 26,  5, 18, 31, 10,
                                                             2,  8, 24, 14, 32, 27,  3,  9, 19, 13, 30,  6, 22, 11,  4, 25
                                                        }
                                            };

    private static byte[] BitMaskTable = new byte[]{
                                                        (byte)0x80, 0x40, 0x20, 0x10, 0x08, 0x04, 0x02, 0x01
                                            };

    private static byte[][] NibbleData = new byte[][]{
            new byte[]{
	            (byte)0xef, 0x03, 0x41, (byte)0xfd, (byte)0xd8, 0x74, 0x1e, 0x47,  0x26, (byte) 0xef, (byte) 0xfb, 0x22, (byte) 0xb3, (byte) 0xd8, (byte) 0x84, 0x1e,
	            0x39, (byte)0xac, (byte)0xa7, 0x60, 0x62, (byte)0xc1, (byte) 0xcd, (byte) 0xba,  0x5c, (byte) 0x96, (byte) 0x90, 0x59, 0x05, 0x3b, 0x7a, (byte) 0x85,
	            0x40, (byte)0xfd, 0x1e, (byte) 0xc8, (byte) 0xe7, (byte) 0x8a, (byte) 0x8b, 0x21,  (byte) 0xda, 0x43, 0x64, (byte) 0x9f, 0x2d, 0x14, (byte) 0xb1, 0x72,
	            (byte) 0xf5, 0x5b, (byte) 0xc8, (byte) 0xb6, (byte) 0x9c, 0x37, 0x76, (byte) 0xec,  0x39, (byte) 0xa0, (byte) 0xa3, 0x05, 0x52, 0x6e, 0x0f, (byte) 0xd9,
            }, 
            new byte[]{
	            (byte) 0xa7, (byte) 0xdd, 0x0d, 0x78, (byte) 0x9e, 0x0b, (byte) 0xe3, (byte) 0x95,  0x60, 0x36, 0x36, 0x4f, (byte) 0xf9, 0x60, 0x5a, (byte) 0xa3,
	            0x11, 0x24, (byte) 0xd2, (byte) 0x87, (byte) 0xc8, 0x52, 0x75, (byte) 0xec,  (byte) 0xbb, (byte) 0xc1, 0x4c, (byte) 0xba, 0x24, (byte) 0xfe, (byte) 0x8f, 0x19,
	            (byte) 0xda, 0x13, 0x66, (byte) 0xaf, 0x49, (byte) 0xd0, (byte) 0x90, 0x06,  (byte) 0x8c, 0x6a, (byte) 0xfb, (byte) 0x91, 0x37, (byte) 0x8d, 0x0d, 0x78,
	            (byte) 0xbf, 0x49, 0x11, (byte) 0xf4, 0x23, (byte) 0xe5, (byte) 0xce, 0x3b,  0x55, (byte) 0xbc, (byte) 0xa2, 0x57, (byte) 0xe8, 0x22, 0x74, (byte) 0xce,
            },
            new byte[]{
	            0x2c, (byte) 0xea, (byte) 0xc1, (byte) 0xbf, 0x4a, 0x24, 0x1f, (byte) 0xc2,  0x79, 0x47, (byte) 0xa2, 0x7c, (byte) 0xb6, (byte) 0xd9, 0x68, 0x15,
	            (byte) 0x80, 0x56, 0x5d, 0x01, 0x33, (byte) 0xfd, (byte) 0xf4, (byte) 0xae,  (byte) 0xde, 0x30, 0x07, (byte) 0x9b, (byte) 0xe5, (byte) 0x83, (byte) 0x9b, 0x68,
	            0x49, (byte) 0xb4, 0x2e, (byte) 0x83, 0x1f, (byte) 0xc2, (byte) 0xb5, 0x7c,  (byte) 0xa2, 0x19, (byte) 0xd8, (byte) 0xe5, 0x7c, 0x2f, (byte) 0x83, (byte) 0xda,
	            (byte) 0xf7, 0x6b, (byte) 0x90, (byte) 0xfe, (byte) 0xc4, 0x01, 0x5a, (byte) 0x97,  0x61, (byte) 0xa6, 0x3d, 0x40, 0x0b, 0x58, (byte) 0xe6, 0x3d,
            },
            new byte[]{
	            0x4d, (byte) 0xd1, (byte) 0xb2, 0x0f, 0x28, (byte) 0xbd, (byte) 0xe4, 0x78,  (byte) 0xf6, 0x4a, 0x0f, (byte) 0x93, (byte) 0x8b, 0x17, (byte) 0xd1, (byte) 0xa4,
	            0x3a, (byte) 0xec, (byte) 0xc9, 0x35, (byte) 0x93, 0x56, 0x7e, (byte) 0xcb,  0x55, 0x20, (byte) 0xa0, (byte) 0xfe, 0x6c, (byte) 0x89, 0x17, 0x62,
	            0x17, 0x62, 0x4b, (byte) 0xb1, (byte) 0xb4, (byte) 0xde, (byte) 0xd1, (byte) 0x87,  (byte) 0xc9, 0x14, 0x3c, 0x4a, 0x7e, (byte) 0xa8, (byte) 0xe2, 0x7d,
	            (byte) 0xa0, (byte) 0x9f, (byte) 0xf6, 0x5c, 0x6a, 0x09, (byte) 0x8d, (byte) 0xf0,  0x0f, (byte) 0xe3, 0x53, 0x25, (byte) 0x95, 0x36, 0x28, (byte) 0xcb,
            }
        };


    /// <summary>
    /// Decodiert einen Pfad oder Dateinamen in einer GRF der Version 1XX
    /// </summary>
    /// <param name="buffer">Der codierte Dateiname oder Pfad.</param>
    public static void decodeName( byte[] buffer ) {
        for( int i = 0; i < buffer.length; i += 8 ) {
            DES.NibbleSwap( buffer, i, 8 );
            DES.BitConvert( buffer, i, 0 );
            DES.BitConvert4( buffer, i );
            DES.BitConvert( buffer, i, 1 );
        }
    }

    /// <summary>
    /// Decodiert den Inhalt einer GRF Datei der Version 1XX
    /// </summary>
    /// <param name="compressed">Der verschlüsselte Inhalt der Datei.</param>
    /// <param name="cycle">Ein Parameter der diverse Auswirkungen auf den beinhalteten Algorithmus hat.</param>
    /*public static void decodeGRF( byte[] compressed, int cycle ) {
        if( cycle == 0 ) {
            for( int i = 0, offset = 0; i < 20 && i * 8 < compressed.Length; i++, offset += 8 ) {
                BitConvert( compressed, offset, 0 );
                BitConvert4( compressed, offset );
                BitConvert( compressed, offset, 1 );
            }
        } else {
            byte[] tmp;

            if( cycle < 3 )
                cycle = 3;
            else if( cycle < 5 )
                cycle++;
            else if( cycle < 7 )
                cycle += 9;
            else
                cycle += 15;

            for( int i = 0, count = 0, offset = 0; i * 8 < compressed.Length; i++, offset += 8 ) {
                if( i < 20 || i % cycle == 0 ) { // des
                    BitConvert( compressed, offset, 0 );
                    BitConvert4( compressed, offset );
                    BitConvert( compressed, offset, 1 );
                } else if( count == 7 ) {
                    tmp = new byte[7];

                    for( int j = 0; j < 7; j++ )
                        tmp[j] = compressed[offset + j];
                    count = 1;
                    compressed[offset + 0] = tmp[3];
                    compressed[offset + 1] = tmp[4];
                    compressed[offset + 2] = tmp[6];
                    compressed[offset + 3] = tmp[0];
                    compressed[offset + 4] = tmp[1];
                    compressed[offset + 5] = tmp[2];
                    compressed[offset + 6] = tmp[5];

                    switch( compressed[offset + 7] ) {
                        case 0x00:
                            compressed[offset + 7] = 0x2b;
                            break;
                        case 0x2b:
                            compressed[offset + 7] = 0x00;
                            break;

                        case 0x01:
                            compressed[offset + 7] = 0x68;
                            break;
                        case 0x68:
                            compressed[offset + 7] = 0x01;
                            break;


                        case 0x48:
                            compressed[offset + 7] = 0x77;
                            break;
                        case 0x77:
                            compressed[offset + 7] = 0x48;
                            break;

                        case 0x60:
                            compressed[offset + 7] = 0xff;
                            break;
                        case 0xff:
                            compressed[offset + 7] = 0x60;
                            break;

                        case 0x6c:
                            compressed[offset + 7] = 0x80;
                            break;
                        case 0x80:
                            compressed[offset + 7] = 0x6c;
                            break;

                        case 0xb9:
                            compressed[offset + 7] = 0xc0;
                            break;
                        case 0xc0:
                            compressed[offset + 7] = 0xb9;
                            break;

                        case 0xeb:
                            compressed[offset + 7] = 0xfe;
                            break;
                        case 0xfe:
                            compressed[offset + 7] = 0xeb;
                            break;
                    }
                } else {
                    count++;
                }
            }
        }
    }
    */
    public static void decodeGRF( byte[] compressed, int cycle ) {
        /*if( cycle == 0 ) {
            for( int i = 0, offset = 0; i < 20 && i * 8 < compressed.Length; i++, offset += 8 ) {
                BitConvert( compressed, offset, 0 );
                BitConvert4( compressed, offset );
                BitConvert( compressed, offset, 1 );
            }
        } else {*/
            boolean type = cycle == 0;

            byte[] tmp;

            if( cycle < 3 )
                cycle = 3;
            else if( cycle < 5 )
                cycle++;
            else if( cycle < 7 )
                cycle += 9;
            else
                cycle += 15;

            for( int i = 0, count = 0, offset = 0; i * 8 < compressed.length; i++, offset += 8 ) {
                if( i < 20 || ( !type && i % cycle == 0 ) ) { // des
                    BitConvert( compressed, offset, 0 );
                    BitConvert4( compressed, offset );
                    BitConvert( compressed, offset, 1 );
                } else if( ( !type && count == 7 ) ) {
                    tmp = new byte[7];

                    for( int j = 0; j < 7; j++ )
                        tmp[j] = compressed[offset + j];
                    count = 1;
                    compressed[offset + 0] = tmp[3];
                    compressed[offset + 1] = tmp[4];
                    compressed[offset + 2] = tmp[6];
                    compressed[offset + 3] = tmp[0];
                    compressed[offset + 4] = tmp[1];
                    compressed[offset + 5] = tmp[2];
                    compressed[offset + 6] = tmp[5];

                    switch( compressed[offset + 7] ) {
                        case 0x00:
                            compressed[offset + 7] = 0x2b;
                            break;
                        case 0x2b:
                            compressed[offset + 7] = 0x00;
                            break;

                        case 0x01:
                            compressed[offset + 7] = 0x68;
                            break;
                        case 0x68:
                            compressed[offset + 7] = 0x01;
                            break;


                        case 0x48:
                            compressed[offset + 7] = 0x77;
                            break;
                        case 0x77:
                            compressed[offset + 7] = 0x48;
                            break;

                        case 0x60:
                            compressed[offset + 7] = (byte)0xff;
                            break;
                        case (byte)0xff:
                            compressed[offset + 7] = 0x60;
                            break;

                        case 0x6c:
                            compressed[offset + 7] = (byte)0x80;
                            break;
                        case (byte)0x80:
                            compressed[offset + 7] = 0x6c;
                            break;

                        case (byte)0xb9:
                            compressed[offset + 7] = (byte)0xc0;
                            break;
                        case (byte)0xc0:
                            compressed[offset + 7] = (byte)0xb9;
                            break;

                        case (byte)0xeb:
                            compressed[offset + 7] = (byte)0xfe;
                            break;
                        case (byte)0xfe:
                            compressed[offset + 7] = (byte)0xeb;
                            break;
                    }
                } else {
                    count++;
                }
            }
        //}
    }

    /// <summary>
    /// Ein spezieller Algorithmus für die GRF Verschlüsselung - Von eAthena abgeschaut
    /// </summary>
    /// <param name="buffer">Das byte Array auf das der Algorithmus angewandt werden soll.</param>
    /// <param name="offset">Gibt an ab welchem Index der Algorithmus angewandt werden soll.</param>
    /// <param name="bitswap">Gibt an welche Tauschtabelle verwendet werden soll.</param>
    private static void BitConvert( byte[] buffer, int offset, int bitswap ) {
        byte[] tmp = new byte[8];

        for( int i = 0; i < 8; i++ )
            tmp[i] = 0x00;

        for( int i = 0; i != 64; i++ ) {
            int prm = ( int )BitSwapTable[bitswap][i] - 1;

            if( ( buffer[offset + ( ( prm >> 3 ) & 7 )] & BitMaskTable[prm & 7] ) != 0 ) {
                tmp[( i >> 3 ) & 7] |= BitMaskTable[i & 7];
            }
        }

        for( int i = 0; i < 8; i++ )
            buffer[offset + i] = tmp[i];
    }

    /// <summary>
    /// Tauscht die Nibble der einzelnen Bytes in einem byte Array.
    /// </summary>
    /// <param name="buffer">Das byte Array in dem die Nibble vertauscht werden sollen.</param>
    /// <param name="offset">Gibt an ab welchem Index die Nibble vertauscht werden sollen.</param>
    /// <param name="length">Gibt die Anzahl der zu tauschenden Nibble an.</param>
    private static void NibbleSwap( byte[] buffer, int offset, int length ) {
        for( int i=0; i < length; i++ ) {
            buffer[offset + i] = ( byte )( ( buffer[offset + i] >> 4 ) | ( buffer[offset + i] << 4 ) );
        }
    }

    /// <summary>
    /// Ein spezieller Algorithmus für die GRF Verschlüsselung - Von eAthena abgeschaut
    /// </summary>
    /// <param name="buffer">Das byte Array auf das der Algorithmus angewandt werden soll.</param>
    /// <param name="offset">Gibt an ab welchem Index der Algorithmus angewandt werden soll.</param>
    private static void BitConvert4( byte[] buffer, int offset ) {
        byte[] tmp = new byte[8];
        tmp[0] = ( byte )( ( ( buffer[offset + 7] << 5 ) | ( buffer[offset + 4] >> 3 ) ) & 0x3f );
        tmp[1] = ( byte )( ( ( buffer[offset + 4] << 1 ) | ( buffer[offset + 5] >> 7 ) ) & 0x3f );
        tmp[2] = ( byte )( ( ( buffer[offset + 4] << 5 ) | ( buffer[offset + 5] >> 3 ) ) & 0x3f );
        tmp[3] = ( byte )( ( ( buffer[offset + 5] << 1 ) | ( buffer[offset + 6] >> 7 ) ) & 0x3f );
        tmp[4] = ( byte )( ( ( buffer[offset + 5] << 5 ) | ( buffer[offset + 6] >> 3 ) ) & 0x3f );
        tmp[5] = ( byte )( ( ( buffer[offset + 6] << 1 ) | ( buffer[offset + 7] >> 7 ) ) & 0x3f );
        tmp[6] = ( byte )( ( ( buffer[offset + 6] << 5 ) | ( buffer[offset + 7] >> 3 ) ) & 0x3f );
        tmp[7] = ( byte )( ( ( buffer[offset + 7] << 1 ) | ( buffer[offset + 4] >> 7 ) ) & 0x3f );

        for( int i = 0; i != 4; i++ )
            tmp[i] = ( byte )( ( ( int )NibbleData[i][( int )tmp[i * 2]] & 0xf0 ) | ( ( int )NibbleData[i][( int )tmp[i * 2 + 1]] & 0x0f ) );

        for( int i = 4; i < 8; i++ )
            tmp[i] = 0;

        for( int i = 0; i != 32; i++ ) {
            int prm = BitSwapTable[2][i] - 1;
            if( ( tmp[prm >> 3] & BitMaskTable[prm & 7] ) != 0 ) {
                tmp[( i >> 3 ) + 4] |= BitMaskTable[i & 7];
            }
        }
        buffer[offset + 0] ^= tmp[4];
        buffer[offset + 1] ^= tmp[5];
        buffer[offset + 2] ^= tmp[6];
        buffer[offset + 3] ^= tmp[7];
    }
}