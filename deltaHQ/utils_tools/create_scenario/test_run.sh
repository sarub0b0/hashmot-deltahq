export PTHREAD_NUM=$1 && ../deltaQ/deltaQ/deltaQ -t $3.xml && mv $3.xml.out $3.xml.out.th$1
export PTHREAD_NUM=$2 && ../deltaQ/deltaQ/deltaQ -t $3.xml && mv $3.xml.out $3.xml.out.th$2
