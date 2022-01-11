import java.io.FileOutputStream;
import java.io.IOException;
import java.util.Random;
import java.util.stream.LongStream;

public class Debug {

    private static Random getSlimeRandom(long worldSeed, int x, int z) {
        long slimeSeed = worldSeed +
                (int) (x * x * 0x4c1906) +
                (int) (x * 0x5ac0db) +
                (int) (z * z) * 0x4307a7L +
                (int) (z * 0x5f24f) ^ 0x3ad8025fL;

        return new Random(slimeSeed);
    }

    public static void main(String[] args) throws IOException {

        // Generate a .bin-File with unique filename containing accurate slime chunk data for the given parameters
        // move generated .bin file to root directory of C++ project
        long worldSeed = 123;

        long width = 1;
        long height = 1;

        int start_x = 23716;
        int start_z = 32867;

        byte[] values = new byte[(int) (width * height)];

        long length = width * height;

        LongStream.range(0, length).parallel().forEach(pos -> {
            int x = (int) (pos % width);
            int z = (int) (pos / width);

            Random rnd = getSlimeRandom(worldSeed, start_x + x, start_z + z);

            values[(int) (z * width + x)] = (byte) (rnd.nextInt(10) == 0 ? 1 : 0);
        });

        try (FileOutputStream fos = new FileOutputStream(String.format("%d-%d-%d-%d-%d.bin", worldSeed, start_x, start_z, width, height))) {
            fos.write(values, 0, values.length);
        }
    }
}
