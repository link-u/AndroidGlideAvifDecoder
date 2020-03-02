package jp.co.link_u.library.glideavif

import android.graphics.Bitmap
import com.bumptech.glide.load.Options
import com.bumptech.glide.load.ResourceDecoder
import com.bumptech.glide.load.engine.Resource
import com.bumptech.glide.load.resource.SimpleResource
import java.io.IOException
import java.nio.ByteBuffer

class AvifDecoderFromByteBuffer : ResourceDecoder<ByteBuffer, Bitmap> {
    override fun handles(source: ByteBuffer, options: Options): Boolean {
        return true
    }

    override fun decode(
        source: ByteBuffer,
        width: Int,
        height: Int,
        options: Options
    ): Resource<Bitmap>? {
        try {
            val bitmap =
                decodeAvif(source, source.remaining())
                    ?: throw DecodeException("avif decode failed")
            return SimpleResource(bitmap)
        } catch (ex: Throwable) {
            throw IOException("Cannot load Avif from stream", ex)
        }
    }

    private external fun decodeAvif(
        byteBuffer: ByteBuffer,
        byteBufferLength: Int
    ): Bitmap?

    companion object {
        init {
            System.loadLibrary("dav1d")
            System.loadLibrary("avif")
            System.loadLibrary("yuv")
            System.loadLibrary("avif_decoder")
        }
    }
}

