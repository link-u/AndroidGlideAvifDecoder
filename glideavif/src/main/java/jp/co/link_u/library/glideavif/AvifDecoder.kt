package jp.co.link_u.library.glideavif

import android.graphics.Bitmap
import android.os.Build
import com.bumptech.glide.load.Options
import com.bumptech.glide.load.ResourceDecoder
import com.bumptech.glide.load.engine.Resource
import com.bumptech.glide.load.resource.SimpleResource
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
            val avif = Avif.getFromInputStream(source)
            return SimpleResource(avif.bitmap)
        } catch (ex: Throwable) {
            throw IOException("Cannot load Avif from stream", ex)
        }
    }

    companion object {
        fun available(): Boolean {
            return Build.VERSION.SDK_INT >= 21 &&
                    Build.SUPPORTED_ABIS.any { it == "arm64-v8a" }
        }
    }
}

