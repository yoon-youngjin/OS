
import java.util.Random;


public class Threadproject extends Thread{
	private static Integer[] buffer = new Integer[10];
	static int in =0;
	static int out =0;
	static int n=buffer.length;

	private static void consumer() throws InterruptedException {
		int nextc;
		for(int i =0;i<10;i++) {
			while(in==out); 
				nextc = buffer[out];
				buffer[out] =null;
				out++;
				out %= n;
				System.out.println("Consume"+nextc);
				Thread.sleep(2000);
				
			}
		
		}
	
	public static void main(String[] args) throws InterruptedException {
		Random random = new Random();
		int nextp;
		int i;
		Thread t2 = new Thread(new Runnable() {

			@Override
			public void run() {
				try {
					consumer();
				}
				catch(Exception e) {
					e.printStackTrace();	}}	
		});
		t2.start();
		
		
		for(i=0;i<10;i++) {
			nextp = random.nextInt(100);
			System.out.println("Produce"+nextp);
			while((in+1)%n == out);
			buffer[in] = nextp;
			in++;
			in%=n;
			Thread.sleep(1000);
		}
		t2.join();
		
		}
	}

		
		
		
	

