/**
 * resize.c
 * Malkevych Roman
 * Computer Science 50
 * Problem Set 4
 * Change size bmp image
 */
       
#include <stdio.h>
#include <stdlib.h>

#include "bmp.h"

int main(int argc, char* argv[])
{
    // ensure proper usage
    if (argc != 4)
    {
        printf("Usage: ./copy infile number outfile\n");
        return 1;
    }

    // Change bmp image to n-size
    int n = atoi(argv[1]);
    
    // remember filenames
    char* infile = argv[2];
    char* outfile = argv[3];
    
    if (n <= 0 || n > 100)
    {
        printf("print another number!");
        return 2;
    }

    // open input file 
    FILE* inptr = fopen(infile, "r");
    if (inptr == NULL)
    {
        printf("Could not open %s.\n", infile);
        return 3;
    }

    // open output file
    FILE* outptr = fopen(outfile, "w");
    if (outptr == NULL)
    {
        fclose(inptr);
        fprintf(stderr, "Could not create %s.\n", outfile);
        return 4;
    }

    // read infile's BITMAPFILEHEADER
    BITMAPFILEHEADER bf;
    fread(&bf, sizeof(BITMAPFILEHEADER), 1, inptr);

    // read infile's BITMAPINFOHEADER
    BITMAPINFOHEADER bi;
    fread(&bi, sizeof(BITMAPINFOHEADER), 1, inptr);

    // ensure infile is (likely) a 24-bit uncompressed BMP 4.0
    if (bf.bfType != 0x4d42 || bf.bfOffBits != 54 || bi.biSize != 40 || 
        bi.biBitCount != 24 || bi.biCompression != 0)
    {
        fclose(outptr);
        fclose(inptr);
        fprintf(stderr, "Unsupported file format.\n");
        return 5;
    }
    
    // Old Width and Height
    int oldWidth = bi.biWidth;
    int oldHeight = bi.biHeight;
    
    //New Width and Height
    bi.biWidth *= n;
    bi.biHeight *= n;
    
    // oldPadding and new padding
    int oldPadding = (4 - (oldWidth * sizeof(RGBTRIPLE)) % 4) % 4;
    int padding = (4 - (bi.biWidth * sizeof(RGBTRIPLE)) % 4) % 4;
    
    // update Image size
    bi.biSizeImage = (bi.biWidth * sizeof(RGBTRIPLE) + padding) * abs(bi.biHeight);

    // edit outfile's BITMAPFILEHEADER to account for resize
    bf.bfSize = (bi.biSizeImage) + 54;
    
    // write outfile's BITMAPFILEHEADER
    fwrite(&bf, sizeof(BITMAPFILEHEADER), 1, outptr);
    
    // write outfile's BITMAPINFOHEADER
    fwrite(&bi, sizeof(BITMAPINFOHEADER), 1, outptr);

    // iterate over infile's scanlines
    for (int i = 0; i < abs(oldHeight); i++)
    { 
        // write to the new file n-lines of pixels
        for (int f = 0; f < n; f++)
        {   
            // skip over padding, if any
            fseek(inptr, (54 + ((oldWidth * 3 + oldPadding) * i)), SEEK_SET);
            
            // iterate over pixels in scanline
            for (int j = 0; j < oldWidth; j++)
            {
                // temporary storage
                RGBTRIPLE triple;
                
                // read RGB triple from infile
                fread(&triple, sizeof(RGBTRIPLE), 1, inptr);

                // write RGB triple to outfile
                for(int a = 0; a < n; a++)
                    fwrite(&triple, sizeof(RGBTRIPLE), 1, outptr);
            }

            // then add it back (to demonstrate how)           
            for (int k = 0; k < padding; k++)
                fputc(0x00, outptr);
        }
    }

    // close infile
    fclose(inptr);

    // close outfile
    fclose(outptr);

    // that's all folks
    return 0;
}
