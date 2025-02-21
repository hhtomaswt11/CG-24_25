# Fase 1 - Primitivas Gráficas

### Forma de execução
- **Generator** 

###### Na diretoria Fase1, de modo a gerar os ficheiros de build, executar os comandos:


```text
mkdir build; cd build ; cmake .. ; cmake --build .
```

Desta forma, os executáveis generator.exe e executable.exe são gerados. 

###### Gerar os ficheiros .3d que definem as primitivas desejadas a construir com o comando:

```text
./generator box 2 3 box_2_3.3d
```

- **Engine** 

###### Executar engine com o comando:

```text
./engine ../test_files_phase_1/test_1_4.xml
```
###### Comando completo (deve ser executado na diretoria Fase1)
```text
mkdir build; cd build ; cmake .. ; cmake --build .  ; ./generator box 2 3 box_2_3.3d  ; ./engine ../test_files_phase_1/test_1_4.xml ; cd ..
```
