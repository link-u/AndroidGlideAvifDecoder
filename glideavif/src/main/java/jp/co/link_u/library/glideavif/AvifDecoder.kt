package jp.co.link_u.library.glideavif

import com.bumptech.glide.load.Options
import com.bumptech.glide.load.ResourceDecoder
import com.bumptech.glide.load.engine.Resource
import com.bumptech.glide.load.resource.SimpleResource
import java.io.IOException
import java.io.InputStream

class AvifDecoder : ResourceDecoder<InputStream, Avif> {
    override fun handles(source: InputStream, options: Options): Boolean {
        // TODO: Can we tell?
        return true
    }

    override fun decode(
        source: InputStream,
        width: Int,
        height: Int,
        options: Options
    ): Resource<Avif>? {
        try {
            val avif = Avif.getFromInputStream(source)
            return SimpleResource(avif)
        } catch (ex: Throwable) {
            throw IOException("Cannot load Avif from stream", ex);
        }
    }
}

