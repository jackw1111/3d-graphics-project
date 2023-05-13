import asyncio
import concurrent.futures
import time

def blocking_io():
    for i in range(30):
        print ("hello world")
        time.sleep(1)

def cpu_bound():
    # CPU-bound operations will block the event loop:
    # in general it is preferable to run them in a
    # process pool.
    return sum(i * i for i in range(10 ** 7))

async def main():
    loop = asyncio.get_running_loop()

    ## Options:

    # 1. Run in the default loop's executor:
    # result = await loop.run_in_executor(
    #     None, blocking_io)
    # print('default thread pool', result)


    # 3. Run in a custom process pool:
    with concurrent.futures.ProcessPoolExecutor() as pool:
        result = await loop.run_in_executor(
            pool, cpu_bound)
        print('custom process pool', result)

asyncio.run(main())