# particiones-dinamicas


## Comandos útiles

* **Crear ejecutables:** `gcc Inicializador.c -o inicializador.o -lpthread && gcc ProductorProcesos.c -o productorProcesos.o -lpthread && gcc Espia.c -o espia.o -lpthread && gcc Finalizador.c -o finalizador.o -lpthread`


* **Ver los segmentos de memoria compartida:**
`ipcs -m`

* **Eliminar los segmentos de memoria compartida:** `ipcrm -m ID`