docker build -t t1_lab_redes . && docker run --name t1_lab_redes_container --net="host" -it t1_lab_redes:latest