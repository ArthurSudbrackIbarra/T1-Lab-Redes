FROM gcc:4.9

WORKDIR /T1_Lab_Redes

COPY src/ ./

RUN gcc -Wall -c map.c
RUN gcc -Wall -c main.c
RUN gcc -o main map.o main.o

EXPOSE 1-2000

CMD ["./main"]