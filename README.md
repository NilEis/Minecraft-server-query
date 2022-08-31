# Minecraft server query

A libary for querying minecraft servers using a [protocol](https://dinnerbone.com/blog/2011/10/14/minecraft-19-has-rcon-and-query/) that is inspired by the [UT3 query protocol](https://wiki.unrealadmin.org/UT3_query_protocol).


## API Reference

#### Initialization

```C
    //Initialize the libary and check for errors
    int ret = msq_init(SERVER_IP, PORT);
    if (ret != 0)
    {
        printf("Error: %s\n", msq_get_error(ret));
    }
```

| Parameter   | Type           | Example       | Description                  |
| :---------- | :------------- | :------------ | :--------------------------- |
| `SERVER_IP` | `const char*`  | `"localhost"` | The ip address of the server |
| `PORT`      | `int`          | `25565`       | The query port of the server |

#### How to get stats
There are two requests that can be made
##### Basic stats
```C
    //The struct will be used to receive server stats
    msq_basic_stats_t basic_stats_struct;
    //Get the stats and check for errors
    ret = msq_get_basic_stats(&basic_stats_struct);
    if (ret != 0)
    {
        printf("Error: %s\n", msq_get_error(ret));
    }
    else
    {
        //If the request was successful, print the stats
        //and free allocated memory
        msq_print_basic_stats(&basic_stats_struct);
        msq_free_basic_stats(&basic_stats_struct);
    }
```
##### Full stats
```C
    //The struct will be used to receive full server stats
    msq_full_stats_t full_stats_struct;
    //Get the stats and check for errors
    ret = msq_get_full_stats(&basic_full_struct);
    if (ret != 0)
    {
        printf("Error: %s\n", msq_get_error(ret));
    }
    else
    {
        //If the request was successful, print the stats
        //and free allocated memory
        msq_print_basic_stats(&full_stats_struct);
        msq_free_basic_stats(&full_stats_struct);
    }
```
#### After usage
```C
//Free allocated ressources
msq_free();
```
