import java.util.concurrent.*;
import java.util.*;

public class MergeSort_FJ extends RecursiveAction {
	static int ThresHold = 4;
	int low;
	int high;
	int[] array;

	public MergeSort_FJ(int l, int h, int[] A) {
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
		/*size > 32, 归并排序*/
		else{
            int mid = (high+low)/2;
			MergeSort_FJ subtask1 = new MergeSort_FJ(low, mid, array);
			MergeSort_FJ subtask2 = new MergeSort_FJ(mid+1, high, array);
			
			subtask1.fork();
			subtask2.fork();
			
			subtask1.join();
			subtask2.join();
			
			int[] tmp = new int [high-low+1];
			int i = low, j=mid+1, k=0;
			while ( i <= mid && j <= high){
				if (array[i] < array[j])
					tmp[k++] = array[i++];
				else
					tmp[k++] = array[j++];
			}
			while (i <= mid)
				tmp[k++] = array[i++];
			while (j <= high)
				tmp[k++] = array[j++];

			for(int s = low; s <= high; s++)
				array[s] = tmp[s-low];
		}
	}

	public static void main(String[] args){
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
		
		MergeSort_FJ task = new MergeSort_FJ(0, size - 1, array);
		Future result = pool.submit(task);
        try{
            System.out.println("result:" + result.get());
        }
        catch (InterruptedException e){
            e.printStackTrace();
        }
        catch (ExecutionException e){
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
