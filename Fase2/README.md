# Fase 2 - Transformações Geométricas

### Forma de execução
- **Generator** 

###### Na diretoria Fase2, de modo a gerar os ficheiros de build, executar os comandos:


```text
mkdir build; cd build ; cmake .. ; cmake --build .
```

Desta forma, os executáveis generator e executable são gerados. 

###### Gerar os ficheiros .3d que definem as primitivas desejadas a construir com o comando:

```text
./generator box 2 3 box.3d
```

##### Nova primitiva - Anel de Saturno: 

```text
./generator ring 2 4 0.2 30 30 ring.3d 
```

- **Engine** 

###### Executar engine com o comando:

```text
./engine ../test_files_phase_2/test_2_1.xml
```

###### Comando completo (deve ser executado na diretoria Fase2)
```text
mkdir build; cd build ; cmake .. ; cmake --build .  ; ./generator box 2 3 box.3d  ; ./engine ../test_files_phase_2/test_2_1.xml ; cd ..
```

- **Sistema Solar** 

Gerar as primitivas necessárias. 

```text
./generator sphere 2 90 90 sphere.3d 
./generator ring 2.2 3 4 30 30 ring.3d 
```

Exceutar a engine com o ficheiro do sistema solar. 


```text
./engine ../test_files_phase_2/solar_system_final.xml 
```

*Sistema solar - Escala real*

```text
./engine ../test_files_phase_2/solar_system_real_scale.xml 
```