# Fase 4 - Normais e Coordenandas de Texturas

### Forma de execução
- **Generator** 

###### Na diretoria Fase4, de modo a gerar os ficheiros de build, executar os comandos:


```text
mkdir build; cd build ; cmake .. ; cmake --build .
```

Desta forma, os executáveis generator e executable são gerados. 

###### Gerar os ficheiros .3d que definem as primitivas desejadas a construir com o comando:

```text
./generator box 2 3 box.3d
```

- **Engine** 

###### Executar engine com o comando:

```text
./engine ../test_files_phase_4/test_4_1.xml
```

###### Comando completo (deve ser executado na diretoria Fase3)
```text
mkdir build; cd build ; cmake .. ; cmake --build .  ; ./generator box 2 3 box.3d  ; ./engine ../test_files_phase_4/test_4_1.xml ; cd ..
```

