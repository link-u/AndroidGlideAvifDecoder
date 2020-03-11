# AndroidGlideAvifDecoder

![Build AAR](https://github.com/link-u/AndroidGlideAvifDecoder/workflows/Build%20AAR/badge.svg)

## Installation

Download AAR from ![latest release](https://github.com/link-u/AndroidGlideAvifDecoder/releases)
and put it to your project module's libs.

Insert below line to build.gradle.
```
implementation(name: 'glideavif', ext: 'aar', version: 'your-downloaded-version')
```

Add GradleModule to your project.

## Example

See ![sample](app/src/main/java/jp/co/link_u/library/glideavif/sample).

## Support

AndroidGlideAvifDecoder supports
- depth 8bit, 10bit, and 12bit
- profile 0, 1, and 2
- monochrome
- alpha opacity
- convert yuv to rgb via libyuv (only 8bit)

not supports
- scale, rotate, or crop transform
