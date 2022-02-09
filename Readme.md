# AndroidGlideAvifDecoder

## Installation

Insert below line to build.gradle.
```
repositories {
    ...
    maven { url 'https://github.com/link-u/AndroidGlideAvifDecoder/raw/master/repository' }
    ...
}

...

implementation 'jp.co.link_u.library.glideavif:glideavif:0.9.0'
```

or
```
implementation 'jp.co.link_u.library.glideavif:glideavif-8bpc:0.9.0'
```

`glideavif-8bpc` supports only 8bpc image. But, the library is lighter than full color depth.

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
