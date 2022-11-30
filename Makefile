make: lista_ligada_a.c lista_ligada_b.c lista_ligada_c.c
	gcc -o ListaLigadaA lista_ligada_a.c -lm
	gcc -o ListaLigadaB lista_ligada_b.c -lm
	gcc -o ListaLigadaC lista_ligada_c.c -lm
     
clean:
	rm -f ListaLigadaA ListaLigadaB ListaLigadaC
