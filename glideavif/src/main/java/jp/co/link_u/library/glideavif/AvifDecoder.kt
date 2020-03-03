package jp.co.link_u.library.glideavif

import android.graphics.Bitmap
import com.bumptech.glide.load.Options
import com.bumptech.glide.load.ResourceDecoder
import com.bumptech.glide.load.engine.Resource
import com.bumptech.glide.load.resource.SimpleResource
import java.io.ByteArrayOutputStream
import java.io.IOException
import java.io.InputStream

class AvifDecoder : ResourceDecoder<InputStream, Bitmap> {
    override fun handles(source: InputStream, options: Options): Boolean {
        return true
    }

    override fun decode(
        source: InputStream,
        width: Int,
        height: Int,
        options: Options
    ): Resource<Bitmap>? {
        try {
            val byteArray = toByteArray(source)
            val bitmap =
                decodeAvif(byteArray, byteArray.size)
                    ?: throw DecodeException("avif decode failed")
            return SimpleResource(bitmap)
        } catch (ex: Throwable) {
            throw IOException("Cannot load Avif from stream", ex)
        }
    }

    private fun toByteArray(s: InputStream): ByteArray {
        val buffer = ByteArrayOutputStream()

        val data = ByteArray(16384)
        while (true) {
            val nRead = s.read(data, 0, data.size)
            if (nRead == -1) {
                break
            }

            buffer.write(data, 0, nRead)
        }
        return buffer.toByteArray()
    }

    private external fun decodeAvif(
        byteArray: ByteArray,
        byteArrayLength: Int
    ): Bitmap?

    companion object {
        init {
            System.loadLibrary("avif_decoder")
        }
    }
}

