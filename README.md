# particiones-dinamicas


## Comandos útiles

* **Crear ejecutables:** `gcc Inicializador.c -o inicializador.o && gcc ProductorProcesos.c -o productorProcesos.o && gcc Espia.c -o espia.o && gcc Finalizador.c -o finalizador.o`


* **Ver los segmentos de memoria compartida:**
`ipcs -m`

* **Eliminar los segmentos de memoria compartida:** `ipcrm -m ID`