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

void check(int n) {
  if (n) print("Test pasado correctamente.\n");
  else print("Test no pasado correctamente.\n");
}

int __attribute__ ((__section__(".text.main")))
  main(void)
{
    /* Next line, tries to move value 0 to CR3 register. This register is a privileged one, and so it will raise an exception */
     /* __asm__ __volatile__ ("mov %0, %%cr3"::"r" (0) ); */

	/* test pagefault handler */
	// char *zero = NULL;
	// *zero = 65;
	
	const char *msg = "Welcome to ZeOS\n";
	const char *longmsg = "Semper Platea Consectetur Nostra Vestibulum Odio Tincidunt Suscipit Sem Quisque Posuere Gravida Sociosqu Lorem Etiam Sapien Curabitur Ornare Lacus Suscipit Gravida Sagittis Conubia Dolor Rhoncus Libero Rutrum At Vestibulum Odio Maecenas Ornare Phasellus Fusce Suspendisse Adipiscing Facilisis Curae Conubia Ac Hendrerit Sed Dictum Etiam Litora Augue Facilisis Cursus Auctor Tortor Laoreet Curabitur Morbi Sodales Condimentum Aenean Massa Consequat Sodales Feugiat Phasellus Consectetur Nisi Euismod Ac Nam Fusce Lectus Tempor Duis Erat Sit Varius Diam Ad Elementum Morbi Viverra Duis Ullamcorper Quis Tristique Dictumst Cubilia Justo Porttitor Ultricies Eu Lacinia Odio Vivamus Vitae Lacinia Integer Etiam Cursus Ullamcorper Dictumst Turpis Est Donec Est Quisque Curabitur Adipiscing Mi Laoreet Odio Class Suspendisse Feugiat Donec Pellentesque Magna Neque Sit Magna Ac Aliquam Massa Enim Praesent Sollicitudin Fringilla Potenti Viverra At Aenean Turpis Eros Lobortis Commodo Vestibulum Accumsan Mi Sociosqu Integer Ultricies Auctor Gravida Blandit Quisque Vel Vulputate Eros Praesent Auctor Integer Aptent Morbi Hendrerit Aliquet Adipiscing Fringilla Condimentum Sollicitudin Accumsan Ipsum Pellentesque Libero Aliquam Ut Ad A Aliquam Facilisis Metus Proin Semper Nullam Faucibus Porta Nulla Dictumst Volutpat Suspendisse Euismod Ut Lacus Justo Et Sociosqu Metus Lacus Proin Lectus Tellus Mattis Dui Litora Tellus Lacinia Tempus Sodales Vivamus Nunc Etiam Tempor Aliquet Magna Nulla Faucibus Laoreet Commodo Nullam Fames Tellus Quisque Vehicula Porttitor Mauris Ullamcorper Viverra Mollis Maecenas Duis Facilisis Purus Quisque Tellus Cras Dolor Ipsum Rutrum Ligula Pharetra Volutpat Nisl Dapibus Curae Ante Id Facilisis Dolor Vitae Venenatis Id Eget Nullam Nulla Ullamcorper Conubia Blandit Suscipit Blandit Lobortis Suspendisse Tristique Adipiscing Faucibus Congue Condimentum Praesent Risus Per Donec Eros Aliquam Nisl Tristique Amet Ut Litora Egestas Risus Sagittis Dolor Volutpat Eget Cubilia Felis Ac Feugiat Volutpat Cursus Curae Metus Enim Nullam Mauris Cursus Suspendisse Cursus Rutrum Facilisis Litora Lacinia Posuere Elementum Consequat Ipsum Egestas Augue Non Elementum Dapibus Aliquam Convallis Vel Enim Nunc Pellentesque Proin Torquent Hendrerit Auctor Diam Netus Ornare Urna Facilisis Praesent Praesent Himenaeos Sit Vitae Sociosqu Venenatis Placerat Netus Luctus Dictum Quisque Porta Sociosqu Aliquet Tincidunt Ante Sagittis Nibh Ut Consequat Phasellus Euismod Nulla Ultricies Accumsan Convallis Fermentum Donec Dui Curae Vulputate Netus Tellus Elementum Venenatis Aliquet Donec Faucibus Curabitur Habitasse Lacinia Phasellus Consectetur Risus Pharetra Ullamcorper Sodales Habitasse Consectetur Neque Nunc Adipiscing Taciti Sit Inceptos Magna Suscipit Sociosqu Egestas Aliquam Iaculis Inceptos Pharetra Molestie At Diam Integer Proin Non Mattis Curabitur Sit Congue Vel Massa Rutrum Habitasse Tortor Condimentum Bibendum Semper Sed Suspendisse Platea Leo Velit At Netus Rhoncus Et At Curabitur Felis Non Ut Enim Felis Tellus Non Facilisis Magna Aenean Rutrum Ultrices Nibh Phasellus Mi Leo Ornare Pulvinar Nulla Tincidunt Magna Bibendum Interdum Pulvinar Vel Etiam Aenean Praesent Ultricies Ornare Posuere Ligula Pharetra At Class Nibh Ac Fringilla Bibendum Hendrerit Mauris Adipiscing Fames Consectetur Adipiscing Suscipit Lorem Aptent Risus Sociosqu Fringilla Lobortis Praesent Ultricies Ultricies Ultricies Eros Torquent Malesuada Ullamcorper Ut Inceptos Enim Mauris Quam Bibendum Vitae A Neque Egestas Fermentum Commodo Hac Habitasse Luctus Et Vestibulum Nec Netus Ullamcorper Convallis Torquent Ornare Cubilia Himenaeos Tristique Morbi Himenaeos Fusce Hac Feugiat Potenti Sapien Consectetur Maecenas Scelerisque Etiam Porta Litora Suscipit Quisque Donec Tortor Tortor Sit Augue Feugiat Interdum Curabitur Hendrerit Scelerisque Ad Placerat Eleifend Feugiat Bibendum Iaculis Nulla Faucibus Interdum Taciti Ut Facilisis Ullamcorper Amet Bibendum Mauris Cursus Habitasse Velit Sagittis Ultricies Elementum Euismod Tincidunt Habitant Nisi Sapien Sagittis Auctor Dapibus Felis Vehicula Dapibus Nulla Ullamcorper Torquent Porta Aliquet Luctus Duis Vestibulum Fringilla Varius Lorem Rhoncus Ultrices Aenean Dictum Vehicula Justo Quam In Vivamus Commodo Purus Venenatis Class Cras Sit Cursus Lacus Eros Etiam Hendrerit Rutrum Tristique Luctus Netus Consequat Maecenas Tristique Etiam Rhoncus Fringilla Aenean Sit Platea Fermentum Feugiat Tempor Quisque Tincidunt Dui Primis Curae Nam Senectus Est Amet Nullam Lobortis Risus Ut Curae Dictum Erat Aenean Id Vehicula Aliquet Laoreet Ultricies Proin Laoreet Luctus Aenean Vitae Egestas Ac Habitant Nulla Erat Ultricies Iaculis Auctor Volutpat Nam Nibh Quisque Pellentesque Semper Dictum Metus Morbi Bibendum Nostra Nulla Eu Consectetur Condimentum Lobortis Eleifend Congue Primis Neque At Aliquet Tortor Quisque Gravida Magna Torquent Primis Sodales Inceptos Quam Urna Maecenas Ultrices Vehicula Quam Ante Vulputate Habitant Cursus Nostra Ipsum Imperdiet Vitae Id Potenti Sed Lacinia Donec Arcu Netus Inceptos A Proin Ac Est Aenean Tellus Quisque Blandit Bibendum Vulputate Phasellus Curae Mi Fringilla Litora Sodales Suspendisse Justo Ornare Dui Hendrerit Faucibus Massa Varius Ut Venenatis Libero Quis Rhoncus Tincidunt Mi Morbi Odio Curabitur Vivamus Curabitur Mattis Facilisis Nullam Neque Ad Imperdiet Sagittis Donec Cras Quisque Purus Aliquam Vel Taciti Felis Lectus Potenti Rhoncus Gravida Mauris Porttitor Vel Orci Sociosqu Ultrices Enim Ac Varius Risus Iaculis Varius Nec Integer Tortor Orci Inceptos Est Ad Habitant Maecenas Urna Ipsum Sociosqu Dapibus Facilisis Platea Torquent Euismod Nibh Aenean Nostra Auctor Nullam Vestibulum Viverra Enim Praesent Augue Diam Nisl Curae Aenean Eleifend Etiam Rutrum Morbi Interdum Fermentum Euismod Lectus Metus Id Vivamus Auctor Odio Vitae Enim Lectus Donec Tortor Himenaeos Primis Phasellus Placerat Taciti Ullamcorper Quisque Curabitur Consectetur Fames Scelerisque Viverra Torquent Ligula Q\n";

	/* Tests sys_write */
	check(write(1, msg, strlen(msg)));

	check(write(0, msg, strlen(msg)) == -1);
	check(write(1, NULL, strlen(msg)));
	check(write(1, msg, -3));
	check(write(1, longmsg, strlen(longmsg)) == strlen(longmsg));

	int ticks = gettime();
	char buf[8];
	hex_to_str(ticks, buf);
	write(1, buf, strlen(buf));
	write(1, "\n", 1);

	while(1);
}
