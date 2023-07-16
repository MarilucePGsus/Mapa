#include <cekeikon.h>

int main() {
    // Lendo as imagens
    Mat_<GRY> ax; le(ax,"janei.pgm");
    Mat_<GRY> ay; le(ay,"janei-1.pgm");
    Mat_<GRY> qx; le(qx,"julho.pgm"); // Imagem de teste
    // Verifica se as imagens de treinamento têm o mesmo
    // tamanho
    if (ax.size()!=ay.size()) erro("Erro dimensao");
    // Cria uma nova matriz vazia com as mesmas dimensões 
    // da imagem de teste
    Mat_<GRY> qp(qx.rows,qx.cols);

    //Cria as estruturas de dados para alimentar OpenCV
    Mat_<FLT> features(ax.rows*ax.cols,9);
    Mat_<FLT> saidas(ax.rows*ax.cols,1);
    // Preenche as matrizes "features" e "saidas" com os 
    // valores das imagens de treinamento, normalizando-os 
    // para o intervalo [0, 1].
    int i=0;
    for (int l=0; l<ax.rows; l++){
        for (int c=0; c<ax.cols; c++) {
                features(i,0)=ax(l-1,c-1)/255.0;
                features(i,1)=ax(l-1,c)/255.0;
                features(i,2)=ax(l-1,c+1)/255.0;
                features(i,3)=ax(l,c-1)/255.0;
                features(i,4)=ax(l,c)/255.0;
                features(i,5)=ax(l,c+1)/255.0;
                features(i,6)=ax(l+1,c-1)/255.0;
                features(i,7)=ax(l+1,c)/255.0;
                features(i,8)=ax(l+1,c+1)/255.0;
            saidas(i)=ay(l,c)/255.0;
            i++;
        }
    }
    // Linhas de 16 a 36 faz conversão das imagens ax e ay
    // para o formato desejado OpenCV
    // Método de pesquisa de vizinho mais próximo
    flann::Index ind(features,flann::KDTreeIndexParams(4));

    Mat_<FLT> query(1,9);
    vector<int> indices(1);
    vector<FLT> dists(1);
    // Loop para percorrer todos os pixels da imagem de teste
    // qx = imagem de teste
    for (int l=0; l<qp.rows; l++){
        // Para cada pixel, cria uma matriz "query" 
        // contendo os valores dos pixels vizinhos na imagem 
        // de teste
        for (int c=0; c<qp.cols; c++) {
                query(0)=qx(l-1,c-1)/255.0;
                query(1)=qx(l-1,c)/255.0;
                query(2)=qx(l-1,c+1)/255.0;
                query(3)=qx(l,c-1)/255.0;
                query(4)=qx(l,c)/255.0;
                query(5)=qx(l,c+1)/255.0;
                query(6)=qx(l+1,c-1)/255.0;
                query(7)=qx(l+1,c)/255.0;
                query(8)=qx(l+1,c+1)/255.0;
            // Zero indica sem backtracking
            ind.knnSearch(query,indices,dists,1,flann::SearchParams(0));
            // Saida eh um numero entre 0 e 255
            // preenche a matriz vazia qp
            qp(l,c)=255*saidas(indices[0]);
        }
    }

    // Filtro para reduzir o ruído de qp
    medianBlur(qp,qp, 7);
    // Converter a matriz qb em uma matriz colorida d
    Mat_<COR> d; converte(qp,d);
    Mat_<COR> output; converte(qx,output);
    imp(qp, "qp.png");
    for (int l=0; l<qp.rows; l++){
        for (int c=0; c<qp.cols; c++) {
            if (qp(l,c) == 0) {
                d(l,c) = COR(0,0,255);
            }
        }
    }
    Mat_<COR> dst;
    // cria a imagem de saída dst
    dst = d*0.3 + output*0.5;
    imp(dst, "julho-c1.png");
}