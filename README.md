# Texture Packer

The Texture Packer tool efficiently combines multiple images into a single optimized texture atlas, reducing memory usage and improving rendering performance. It streamlines asset management for game development and UI design by optimizing space usage with intelligent packing algorithms.

## Key features

- Automatically add images from a folder or via the command line.
- Supports input formats: JPEG, PNG, TGA, BMP, PSD, GIF, HDR, PIC, PNM.
- Exports to PNG (default), TGA, and BMP.
- Ability to trim input images to remove transparent areas.
- Option to set a border around images for better separation.

## Usage

```sh
texpacker INPUT_IMAGE [INPUT_IMAGE] -o ATLAS
  INPUT_IMAGE        input image name or directory separated by space
  -o ATLAS           output atlas name (default PNG)
  -res DESC_TEXTURE  output atlas description as XML
  -pot               make power of two atlas
  -trim              trim sprites
  -overlay           draw overlay over sprite
  -dupes             allow dupes
  -slow              use slow method instead kd-tree
  -b size            add border around sprites
  -p size            add padding between sprites
```

## Download and build

You can browse the source code repository on GitHub or get a copy using git with the following command:
```sh
git clone https://github.com/andreyugolnik/texture-packer.git
make release
```

## Input files notes

- **JPEG** baseline & progressive (12 bpc/arithmetic not supported, same as stock IJG lib).
- **PNG** 1/2/4/8-bit-per-channel (16 bpc not supported).
- **TGA** (not sure what subset, if a subset).
- **BMP** non-1bpp, non-RLE.
- **PSD** (composited view only, no extra channels, 8/16 bit-per-channel).
- **GIF** (*comp always reports as 4-channel).
- **HDR** (radiance rgbE format).
- **PIC** (Softimage PIC).
- **PNM** (PPM and PGM binary only).

***

> Copyright © 2017-2024 Andrey A. Ugolnik. All Rights Reserved.
>
> https://www.ugolnik.info
>
> andrey@ugolnik.info
