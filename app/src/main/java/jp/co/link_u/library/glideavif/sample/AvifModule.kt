package jp.co.link_u.library.glideavif.sample

import android.content.Context
import android.graphics.drawable.BitmapDrawable
import com.bumptech.glide.Glide
import com.bumptech.glide.Registry
import com.bumptech.glide.annotation.GlideModule
import com.bumptech.glide.module.AppGlideModule
import jp.co.link_u.library.glideavif.Avif
import jp.co.link_u.library.glideavif.AvifDecoder
import jp.co.link_u.library.glideavif.AvifDrawableTranscoder
import java.io.InputStream

@GlideModule
class AvifModule : AppGlideModule() {
    override fun registerComponents(context: Context, glide: Glide, registry: Registry) {
        registry
            .register(Avif::class.java, BitmapDrawable::class.java, AvifDrawableTranscoder())
            .append(
                InputStream::class.java,
                Avif::class.java,
                AvifDecoder()
            )
    }

    // Disable manifest parsing to avoid adding similar modules twice.
    override fun isManifestParsingEnabled(): Boolean {
        return false
    }
}
