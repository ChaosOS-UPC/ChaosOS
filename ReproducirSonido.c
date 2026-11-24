#include <stdint.h>
#include <sys/wait.h>
#include <unistd.h>

int ejecutar_decodificador(int pipe_in[2], int pipe_out[2]) {
    close(pipe_in[0]);
    close(pipe_out[0]);
    close(pipe_out[1]);
    dup2(pipe_in[1], STDOUT_FILENO);
    close(pipe_in[1]);

    execlp("gst-launch-1.0", "gst-launch-1.0", "-q",
       "filesrc", "location=sonido.mp3", "!",
       "decodebin", "!", "audioconvert", "!", "audioresample", "!",
       "audio/x-raw,format=S32LE,rate=44100,channels=2", "!",
       "fdsink", "fd=1", NULL);

}

int ejecutar_reproductor(int pipe_in[2], int pipe_out[2]) {
    close(pipe_out[1]);
    close(pipe_in[0]);
    close(pipe_in[1]);
    dup2(pipe_out[0], STDIN_FILENO);
    close(pipe_out[0]);

    execlp("aplay", "aplay", "-q", "-f", "S32_LE", "-r", "44100", "-c", "2", NULL);

}

int procesar_audio(int pipe_in[2], int pipe_out[2]) {
    int buffer[1024];
    int bytes_leidos;
    double volumen = 0.5;

    close(pipe_in[1]);
    close(pipe_out[0]);

    while ((bytes_leidos = read(pipe_in[0], buffer, sizeof(buffer))) > 0) {
        int muestras = bytes_leidos / sizeof(int);
        for (int i = 0; i < muestras; i++) {
            buffer[i] = buffer[i] * volumen;
        }
        write(pipe_out[1], buffer, bytes_leidos);
    }

    close(pipe_in[0]);
    close(pipe_out[1]);
    wait(NULL);
    wait(NULL);
}

int main(void) {
    int pipe_in[2];
    int pipe_out[2];
    int status = 1;

    if (pipe(pipe_in) == 0 && pipe(pipe_out) == 0) {
        pid_t pid1 = fork();

        if (pid1 == 0) {
            status = ejecutar_decodificador(pipe_in, pipe_out);
        } else if (pid1 > 0) {
            pid_t pid2 = fork();

            if (pid2 == 0) {
                status = ejecutar_reproductor(pipe_in, pipe_out);
            } else if (pid2 > 0) {
                status = procesar_audio(pipe_in, pipe_out);
            }
        }
    }

    return 0;
}
