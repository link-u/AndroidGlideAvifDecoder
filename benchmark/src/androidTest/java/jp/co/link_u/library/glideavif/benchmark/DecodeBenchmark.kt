package jp.co.link_u.library.glideavif.benchmark

import android.content.Context
import androidx.benchmark.BenchmarkRule
import androidx.benchmark.measureRepeated
import androidx.test.core.app.ApplicationProvider
import androidx.test.ext.junit.runners.AndroidJUnit4
import com.bumptech.glide.load.Options
import jp.co.link_u.library.glideavif.AvifDecoderFromByteBuffer
import org.junit.Rule
import org.junit.Test
import org.junit.runner.RunWith
import java.io.ByteArrayOutputStream
import java.io.InputStream
import java.nio.ByteBuffer

@RunWith(AndroidJUnit4::class)
class DecodeBenchmark {
    @get:Rule
    val benchmarkRule = BenchmarkRule()

    private val context = ApplicationProvider.getApplicationContext<Context>()

    @Test
    fun simpleDecode() {
        val decoder = AvifDecoderFromByteBuffer()
        val inputStream = context.assets.open("images/fox.profile0.8bpc.yuv420.avif")
        val data = toByteArray(inputStream)
        val buffer = ByteBuffer.allocateDirect(data.size)
        buffer.mark()
        buffer.put(data)
        buffer.reset()

        benchmarkRule.measureRepeated {
            decoder.decode(buffer, 1204, 800, Options())
        }
    }

    private fun toByteArray(s: InputStream): ByteArray {
        var nRead: Int
        val data = ByteArray(16384)

        val buffer = ByteArrayOutputStream()
        while (s.read(data, 0, data.size).also { nRead = it } != -1) {
            buffer.write(data, 0, nRead)
        }

        return buffer.toByteArray()
    }
}
