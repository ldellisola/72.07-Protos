using System.CommandLine;
using System.Net;
using System.Text.RegularExpressions;


long errors = 0;
long attempts = 0;
bool runUsers = true;
var random = new Random();

var socksRegex = new Regex(@"socks5://((?<username>[^:]+):(?<password>[^@]+)@)?(?<address>[^:]+:\d+)");


async Task RunUser(int requestsPerSecond,string target, CancellationToken token)
{
    Console.WriteLine("Starting up user...");
    while (runUsers)
    {
        attempts++;
        try
        {
            var handler = new HttpClientHandler
            {
                AllowAutoRedirect = false
            };
            using var client = new HttpClient(handler);
            var m = await client.GetAsync(target, token);
            if ((int) m.StatusCode >= 400)
                errors++;

            await using (await m.Content.ReadAsStreamAsync(token))
            {
            }
        }
        catch
        {
            errors++;
        }

        double msToWait = 1000 / requestsPerSecond;
        msToWait += 100 * (random.NextSingle() - 0.5);
        
        await Task.Delay((int) msToWait,token);
    }
    Console.WriteLine("Shutting down user...");
}

var socks5 = string.Empty;
var runtime = 10;
var requestsPerSecond = 2;
var users = 2;
var target = string.Empty;

var socks5ServerOption = new Option<string>(
    aliases: new []{"--proxy", "-p"},
    description: "Proxy to test",
    getDefaultValue: () => "socks5://localhost:1080"
) ;

socks5ServerOption.AddValidator(result =>
{
    if (!socksRegex.IsMatch(result.GetValueForOption(socks5ServerOption) ?? ""))
        result.ErrorMessage = "The proxy is not a valid socks5 address";
});

var runtimeOption = new Option<int>(
    aliases: new []{"--runtime","-r"},
    description: "Time to run the test",
    getDefaultValue: () => 60
    );

var requestPerSecondOption = new Option<int>(
    aliases: new[] {"--request-per-second", "-rps"},
    description: "The number of requests per second each user will do",
    getDefaultValue: () => 2
);

var usersOption = new Option<int>(
    aliases: new[] {"--users", "-u"},
    description: "The number of users to run concurrently",
    getDefaultValue: () => 2
);

var targetOption = new Option<string>(
    aliases: new[] {"--target", "-t"},
    description: "The address to make all the calls to"
) {IsRequired = true};

var rootCommand = new RootCommand("This is a small program intended to stress test our socks5d implementation")
    {socks5ServerOption, runtimeOption,requestPerSecondOption,usersOption,targetOption};

rootCommand.SetHandler((s,r,rps,u,t) =>
{
    socks5 = s;
    runtime = r;
    requestsPerSecond = rps;
    users = u;
    target = t;
} ,socks5ServerOption,runtimeOption,requestPerSecondOption,usersOption,targetOption);


var result= await rootCommand.InvokeAsync(args);

// todo see this when calling version.
if (result is not 0 || target == string.Empty)
    return;


Console.WriteLine("-----------------------------------------------");
Console.WriteLine("Starting stress test!");
Console.WriteLine($"Expected runtime:\t{runtime} seconds");
Console.WriteLine($"Number of users:\t{users}");
Console.WriteLine($"Expected requests per second:\t{requestsPerSecond}");
Console.WriteLine($"Target Url:\t{target}");
Console.WriteLine($"Socks5 server:\t{socks5}");
Console.WriteLine("-----------------------------------------------");


var match = socksRegex.Match(socks5);

var proxy = new WebProxy(socks5);

if (match.Groups["username"].Success && match.Groups["password"].Success)
    proxy.Credentials = new NetworkCredential(match.Groups["username"].Value, match.Groups["password"].Value);

HttpClient.DefaultProxy = proxy;

List<Task> allTasks = new List<Task>();

for (var i = 0; i < users; i++)
{
    await Task.Delay(random.Next(300));
    allTasks.Add(RunUser(requestsPerSecond, new UriBuilder(target).ToString(), CancellationToken.None));
}

await Task
    .Run(() => Task.Delay(TimeSpan.FromSeconds(runtime)).Wait())
    .ContinueWith(_ => runUsers = false)
    .ContinueWith(_ => Console.WriteLine("Stopping workers!"));
Task.WaitAll(allTasks.ToArray());

Console.WriteLine("-----------------------------------------------");
Console.WriteLine("Stress test finished!");
Console.WriteLine($"Total attempts: {attempts}");
Console.WriteLine($"Total errors: {errors}");
Console.WriteLine("-----------------------------------------------");








