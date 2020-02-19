package jp.co.link_u.library.glideavif.sample

import android.graphics.drawable.Drawable
import android.os.Bundle
import android.util.Log
import android.view.LayoutInflater
import android.view.ViewGroup
import android.widget.ImageView
import android.widget.LinearLayout
import android.widget.TextView
import androidx.appcompat.app.AppCompatActivity
import androidx.recyclerview.widget.LinearLayoutManager
import androidx.recyclerview.widget.RecyclerView
import com.bumptech.glide.Glide
import com.bumptech.glide.RequestBuilder
import com.bumptech.glide.load.DataSource
import com.bumptech.glide.load.engine.GlideException
import com.bumptech.glide.load.resource.drawable.DrawableTransitionOptions.withCrossFade
import com.bumptech.glide.request.RequestListener
import com.bumptech.glide.request.target.Target

class MainActivity : AppCompatActivity() {
    private lateinit var recyclerView: RecyclerView
    private lateinit var viewAdapter: RecyclerView.Adapter<*>
    private lateinit var viewManager: RecyclerView.LayoutManager

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_main)

        val requestBuilder = Glide.with(this)
            .asDrawable()
            .transition(withCrossFade())

        viewManager = LinearLayoutManager(this)
        viewAdapter = MyAdapter(
            requestBuilder,
            resources.getString(R.string.resource_root),
            resources.getStringArray(R.array.resource_names)
        )

        recyclerView = findViewById<RecyclerView>(R.id.image_list).apply {
            setItemViewCacheSize(6)
            setHasFixedSize(true)
            layoutManager = viewManager
            adapter = viewAdapter
        }
    }

    class MyAdapter(
        private val requestBuilder: RequestBuilder<Drawable>,
        private val resourceRoot: String,
        private val images: Array<String>
    ) :
        RecyclerView.Adapter<MyAdapter.MyViewHolder>() {

        class MyViewHolder(v: LinearLayout) : RecyclerView.ViewHolder(v) {
            val textView: TextView = v.findViewById(R.id.textView)
            val imageView: ImageView = v.findViewById(R.id.imageView)
        }

        override fun onCreateViewHolder(parent: ViewGroup, viewType: Int): MyViewHolder {
            val v = LayoutInflater.from(parent.context)
                .inflate(R.layout.image_cell, parent, false) as LinearLayout
            return MyViewHolder(v)
        }

        override fun onBindViewHolder(holder: MyViewHolder, position: Int) {
            val name = images[position]

            holder.textView.text = name.split(".").joinToString("\n")
            requestBuilder
                .skipMemoryCache(true)
                .listener(LogListener(name))
                .load(resourceRoot + name)
                .into(holder.imageView)
        }

        override fun getItemCount() = images.size
    }

    class LogListener(val name: String) : RequestListener<Drawable> {
        override fun onLoadFailed(
            e: GlideException?,
            model: Any?,
            target: Target<Drawable>?,
            isFirstResource: Boolean
        ): Boolean {
            return false
        }

        override fun onResourceReady(
            resource: Drawable?,
            model: Any?,
            target: Target<Drawable>?,
            dataSource: DataSource?,
            isFirstResource: Boolean
        ): Boolean {
            Log.d("AvifDecoder", name)
            return false
        }
    }
}
