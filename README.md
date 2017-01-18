##Texture Packer##

Texture Packer tool allows you to pack individual images into one texture atlas.

##Features##

* Automatically add all images from folder or from command line.
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

##Dependencies##

* libpng - png format support.

***
```
Copyright © 2017 Andrey A. Ugolnik. All Rights Reserved.
http://www.ugolnik.info
andrey@ugolnik.info
```
