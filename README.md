# T1-Lab-Redes

Trabalho 1 de Laboratório de Redes.

### Como Executar (Docker)

1. Tenha a ferramenta **Docker** instalada em seu dispositivo.

2. Clone este repositório e vá para a o diretório clonado.

3. Digite os comandos abaixo, em ordem:

```sh
docker build -t t1_lab_redes .
```

```sh
docker run --name t1_lab_redes_container --net=host -it t1_lab_redes
```
