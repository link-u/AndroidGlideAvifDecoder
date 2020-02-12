package jp.co.link_u.library.glideavif.sample

import android.content.ContentResolver
import android.graphics.drawable.Drawable
import android.net.Uri
import android.os.Bundle
import android.widget.ImageView
import androidx.appcompat.app.AppCompatActivity
import com.bumptech.glide.Glide
import com.bumptech.glide.RequestBuilder
import com.bumptech.glide.load.resource.drawable.DrawableTransitionOptions.withCrossFade

class MainActivity : AppCompatActivity() {
    private lateinit var imageViewRes: ImageView
    private lateinit var requestBuilder: RequestBuilder<Drawable>

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_main)

        imageViewRes = findViewById(R.id.svg_image_view1)

        requestBuilder = Glide.with(this)
            .asDrawable()
            .transition(withCrossFade())
    }

    override fun onStart() {
        super.onStart()
        val uri = Uri.parse(
            ContentResolver.SCHEME_ANDROID_RESOURCE
                    + "://"
                    + packageName
                    + "/"
                    + R.raw.avif_sample
        )

        requestBuilder.load(uri).into(imageViewRes)
    }
}
