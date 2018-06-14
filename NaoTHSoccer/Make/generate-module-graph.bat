echo "generate modules graph"
premake5 modules
dot -Tpdf "../build/modules.txt" -o graph2.pdf
pause