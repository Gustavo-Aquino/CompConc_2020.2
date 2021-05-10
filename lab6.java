//--Gustavo Martins Ferreira de Aquino
//--DRE:119044012

class buffer_comp {
    private int quant;      //tamanho do vetor
    private int nthreads;   //quant. threads
    public int[] vetor;;
    
    //construtor
    public buffer_comp(int quantidade, int threads){
        this.quant=quantidade;                     
        this.nthreads=threads;
        vetor=new int[quant];
    }

    public void ini(){//inicializa o vetor
        for(int i=0;i<quant;i++) this.vetor[i]=i;
    }

    //funcao usada pelas threads
    public void adiciona(int ini){
        //faz a soma com a intercalacao das threads
        for(int i=ini;i<quant;i+=nthreads) vetor[i]+=1;
    }

    //Como estamos em um problema fechado vamos fazer um teste especifico dado que sabemos a saida correta
    public String teste(){
    	int aux = 1;
        //O vetor de saida deve ser tal que vetor[i]=i++, logo para nosso vetor de 10 posicoes temos [1 2 3 4 5 6 7 8 9 10]
        for(int i=0;i<this.quant;i++){
            if(this.vetor[i]==aux) {
            	aux++;
            }
            else return "Algo esta errado";
        }
        return "Incremento correto";
    }
}

class Thd extends Thread{
    //variaveis da thread
    private int id;
    buffer_comp vec;

    //construtor
    public Thd(int tid, buffer_comp vec){
        this.id=tid;
        this.vec=vec;
    }

    //metodo main
    public void run(){
        this.vec.adiciona(id);
    }

}

class lab6 {
        public static void main (String[] args) {
        //le e valida os parametros passados
        if (args.length < 2) {
            System.out.println("Executar: arquivo <Num. threads> <Tamanho do vetor>"+args.length);
            return;
        }
        int nthreads = Integer.parseInt(args[0]);
        int tamanho = Integer.parseInt(args[1]);

        //reserva espaco para um vetor de threads
        Thread[] threads = new Thread[nthreads];
        //cria uma instancia do recurso compartilhado entre as threads
        buffer_comp vec = new buffer_comp(tamanho,nthreads);
        vec.ini();

        //cria as threads da aplicacao
        for (int i=0; i<threads.length; i++) threads[i] = new Thd(i, vec);
     
        //inicia as threads
        for (int i=0; i<threads.length; i++) threads[i].start();
     
        //espera pelo termino de todas as threads
        for (int i=0; i<threads.length; i++) {
            try { threads[i].join(); } catch (InterruptedException e) { return; }
        }
        //Testa a solucao
        System.out.println(vec.teste());
    }

}


