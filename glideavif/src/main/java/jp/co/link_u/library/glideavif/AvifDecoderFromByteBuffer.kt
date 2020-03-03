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
        return isAvif(source)
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

    private fun isAvif(buf: ByteBuffer): Boolean {
        try {
            while (buf.hasRemaining()) {
                val box = HeaderBox(buf)
                if (box.type.contentEquals(HeaderBox.ftyp)) {
                    return box.data.take(4).toByteArray().contentEquals(HeaderBox.avif)
                }
            }
        } catch (e: Throwable) {
        }
        return false
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

    class HeaderBox(buf: ByteBuffer) {
        var size: Int = 0
            private set
        val type: ByteArray = ByteArray(4)
        var data: ByteArray
            private set

        init {
            size = buf.getInt()

            if (size == 1 || size == 0) {
                throw RuntimeException("invalid size")
            }

            buf.get(type)

            if (!primitives.any { type.contentEquals(it) }) {
                throw RuntimeException("unknown box")
            }

            data = ByteArray(size - 8)
            buf.get(data)
        }

        companion object {
            val ftyp = byteArrayOf(0x66, 0x74, 0x79, 0x70)
            val avif = byteArrayOf(0x61, 0x76, 0x69, 0x66)
            val primitives = arrayOf(ftyp)
        }
    }
}

