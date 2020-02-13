package jp.co.link_u.library.glideavif

import android.graphics.Bitmap
import android.util.Log
import java.io.ByteArrayOutputStream
import java.io.InputStream

class Avif(inputStream: InputStream) {
    var bitmap: Bitmap
        private set

    init {
        val byteArray = toByteArray(inputStream)

        this.bitmap = decodeAvif(byteArray, byteArray.size)
    }

    private external fun decodeAvif(
        byteArray: ByteArray,
        byteArrayLength: Int
    ): Bitmap

    companion object {
        fun getFromInputStream(s: InputStream): Avif {
            return Avif(s)
        }

        private fun toByteArray(s: InputStream): ByteArray {
            val buffer = ByteArrayOutputStream()

            var nRead: Int
            val data = ByteArray(16384)

            while (true) {
                nRead = s.read(data, 0, data.size)
                if (nRead == -1) {
                    break
                }

                buffer.write(data, 0, nRead)
            }
            return buffer.toByteArray()
        }

        // Used to load the 'native-lib' library on application startup.
        init {
            System.loadLibrary("dav1d")
            System.loadLibrary("avif")
            System.loadLibrary("native-lib")
        }
    }
}
