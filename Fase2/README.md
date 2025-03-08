# Fase 2 - Transformações Geométricas

### Forma de execução
- **Generator** 

###### Na diretoria Fase1, de modo a gerar os ficheiros de build, executar os comandos:


```text
mkdir build; cd build ; cmake .. ; cmake --build .
```

Desta forma, os executáveis generator e executable são gerados. 

###### Gerar os ficheiros .3d que definem as primitivas desejadas a construir com o comando:

```text
./generator box 2 3 box_2_3.3d
```

- **Engine** 

###### Executar engine com o comando:

```text
./engine ../test_files_phase_2/test_2_1.xml
```
###### Comando completo (deve ser executado na diretoria Fase1)
```text
mkdir build; cd build ; cmake .. ; cmake --build .  ; ./generator box 2 3 box_2_3.3d  ; ./engine ../test_files_phase_2/test_2_1.xml ; cd ..
```

