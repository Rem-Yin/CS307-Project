import java.util.concurrent.*;
import java.util.*;

public class QuickSort_FJ extends RecursiveAction {
	static int ThresHold = 32;
	int low;
	int high;
	int[] array;

	public QuickSort_FJ(int l, int h, int[] A) {
		this.low = l;
		this.high = h;
		this.array = A;
	}

	protected void compute() {
		/*size <= 32, 选择排序*/
		if (high - low + 1 <= ThresHold) {
			for(int i=low; i <= high; ++i)
			{
				int tmp_min = i;
				for(int j = i; j <= high; ++j)
					if(array[j] < array[tmp_min])
					{
						tmp_min = j;
					}
				int tmp = array[i];
				array[i] = array[tmp_min];
				array[tmp_min] = tmp;
			}
		} 
		/*size > 32, 快速排序*/
		else {
			int tmp = array[low];
			int l = low, r = high;
			while (l < r) {
				while (l < r && array[r] > tmp)
					r--;
				if(l<r)
					array[l++] = array[r];
				else
					break;
					
				while (l < r && array[l] < tmp) 
					l++;
				if(l<r)
					array[r--] = array[l];
				else
					break;
			}
			array[l] = tmp;
            
			QuickSort_FJ subtask1 = new QuickSort_FJ(low, l-1, array);
			QuickSort_FJ subtask2 = new QuickSort_FJ(l+1, high, array);
			
			subtask1.fork();
			subtask2.fork();
			
			subtask1.join();
			subtask2.join();
		}
	}

	public static void main(String[] args) {
		ForkJoinPool pool = new ForkJoinPool();
		Scanner s = new Scanner(System.in);

		/*input size of array*/
		System.out.print("Please input the size of the array: ");
		int size = s.nextInt();
		int [] array = new int [size];

		/*get the array*/
		for (int i = 0; i < size; ++ i){
			System.out.print("Please input the "+i+"th element: ");
			array[i] = s.nextInt();
		}
		
		QuickSort_FJ task = new QuickSort_FJ(0, size - 1, array);
		Future result = pool.submit(task);
        try {
            System.out.println("result:" + result.get());
        } catch (InterruptedException e) {
            e.printStackTrace();
        } catch (ExecutionException e) {
            e.printStackTrace();
        }
		
		/*output the sorted array*/
		System.out.println("The sorted array is:");
		for (int i = 0; i < size; ++ i){
			System.out.print(array[i]+" ");
		}
		System.out.print("\n");
		s.close();
	}
}
