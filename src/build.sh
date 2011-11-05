echo "-----------------------------"
echo "depends on ..."
echo "  bison >= 2.5"
echo "  flex >= 2.5.35"
echo "-----------------------------"
echo ""

export UTILS=../util/string_buffer.c
export UTILS=../util/vector.c\ $UTILS
export UTILS=../util/list.c\ $UTILS
export UTILS=../util/map.c\ $UTILS
export UTILS=../util/exception.c\ $UTILS
export UTILS=../util/set.c\ $UTILS
export UTILS=../util/mem.c\ $UTILS

bison -d -y parser.y
flex lexer.l
gcc -g -o bd $UTILS y.tab.c lex.yy.c type.c env.c id.c show.c main.c compile.c primitives.c expr.c sexpr.c typing.c nexpr.c knormal.c alpha.c flatten.c cexpr.c closure.c
