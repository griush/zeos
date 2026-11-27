#include <libc.h>

char buff[24];
int pid;
int result;

void hex_to_str(unsigned int val, char *buf)
{
	const char digits[] = "0123456789ABCDEF";
	int i;
	for (i = 7; i >= 0; i--) {
		buf[i] = digits[val & 0xF];
		val >>= 4;
	}
	buf[8] = '\0';
}

int print(const char *msg) {
	return write(1, msg, strlen(msg));
}

int __attribute__ ((__section__(".text.main")))
  main(void)
{
	char buf[9];
	print("\n=== TEST READ DE ZEOS ===\nPID actual: ");
	hex_to_str(getpid(), buf);
	print(buf);
	print("\n");

	/* -------------------------------------------------------------
	 * PRUEBA 1: Lectura síncrona bloqueante (5 teclas)
	 * ------------------------------------------------------------- */
	print("\n[PRUEBA 1] Escribe 5 caracteres por teclado para continuar:\n");
	
	char read_buf[32];
	int bytes_leidos = read(read_buf, 5);

	if (bytes_leidos > 0) {
		print("¡Desbloqueado! Has escrito: ");
		write(1, read_buf, bytes_leidos);
		print("\n");
	} else {
		print("Error en sys_read\n");
	}

	/* -------------------------------------------------------------
	 * PRUEBA 2: Concurrencia de lectura con fork() (Orden FIFO)
	 * ------------------------------------------------------------- */
	print("\n[PRUEBA 2] Ejecutando fork()...\n");
	
	int pid = fork();
	if (pid < 0) {
		print("Error al ejecutar fork()\n");
	}

	if (pid == 0) {
		/* --- PROCESO HIJO --- */
		print("[HIJO] Esperando 3 teclas...\n");
		char child_buf[10];
		int ret_hijo = read(child_buf, 3);
		if (ret_hijo > 0) {
			print("[HIJO] Leido con exito: ");
			write(1, child_buf, ret_hijo);
			print("\n[HIJO] Finalizando...\n");
		}
		exit();
	} else {
		/* --- PROCESO PADRE --- */
		print("[PADRE] Esperando 3 teclas...\n");
		char parent_buf[10];
		int ret_padre = read(parent_buf, 3);
		if (ret_padre > 0) {
			print("[PADRE] Leido con exito: ");
			write(1, parent_buf, ret_padre);
			print("\n[PADRE] Finalizando...\n");
		}
	}

	while(1) {
		/* Bucle infinito final */
	}
}