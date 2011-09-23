echo "-----------------------------"
echo "depends on ..."
echo "  bison >= 2.5"
echo "  flex >= 2.5.35"
echo "-----------------------------"
echo ""

bison -d -y parser.y
flex lexer.l
gcc -g -o test y.tab.c lex.yy.c util/string_buffer.c util/vector.c util/list.c util/map.c util/exception.c util/set.c util/mem.c type.c env.c id.c show.c expr.c expr1.c expr2.c expr3.c typing.c knormal.c alpha.c beta.c flatten.c inline.c constfold.c elim.c closure.c program.c
