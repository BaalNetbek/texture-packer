##Texture Packer##

Texture Packer tool allows you to pack individual images into one texture atlas.

##Features##

* Automatically add all images from folder or from command line.
* Supported input formats: JPEG, PNG, TGA, BMP, PSD, GIF, HDR, PIC, PNM.
* Write to: PNG (default), TGA, BMP.
* Ability to set border around image.
* Ability to trim input images.

##Usage##

```
$ packimages INPUT_IMAGE [INPUT_IMAGE] -o ATLAS
  INPUT_IMAGE        input image name or directory separated by space
  -o ATLAS           output atlas name (default PNG)
  -res DESC_TEXTURE  output atlas description as XML
  -pot               make power of two atlas
  -trim              trim sprites
  -overlay           draw overlay over sprite
  -b size            add border around sprites
  -p size            add padding between sprites
  -max size          max atlas size
```

##Download and build##

You can browse the source code repository on GitHub or get a copy using git with the following command:
```
$ git clone https://bitbucket.org/andreyu/texture-packer.git
$ make release
```

##Input files notes##

> JPEG baseline & progressive (12 bpc/arithmetic not supported, same as stock IJG lib). 
> PNG 1/2/4/8-bit-per-channel (16 bpc not supported). 
> TGA (not sure what subset, if a subset). 
> BMP non-1bpp, non-RLE. 
> PSD (composited view only, no extra channels, 8/16 bit-per-channel). 
> GIF (*comp always reports as 4-channel). 
> HDR (radiance rgbE format). 
> PIC (Softimage PIC). 
> PNM (PPM and PGM binary only). 

***
```
Copyright © 2017 Andrey A. Ugolnik. All Rights Reserved.
http://www.ugolnik.info
andrey@ugolnik.info
```
