//extern crate getopts;
extern crate nix;
use getopts::Options;
use std::env;
use std::process;
use nix::sys::signal;
use nix::unistd;
use std::thread;
use std::time;

use nix::unistd::{fork, ForkResult};


fn print_usage(program: &str, opts: &Options) {
    let brief = format!("Usage: {} FILE [options] [PID1 PID2...]", program);
    print!("{}", opts.usage(&brief));
    process::exit(0)
}

fn error(program: &str, opts: &Options, msg: String) -> u64 {
    println!("Error: {}", msg);
    print_usage(&program, opts);
    0
}

fn errori32(program: &str, opts: &Options, msg: String) -> i32 {
    println!("Error: {}", msg);
    print_usage(&program, opts);
    0
}

fn main() {
    let args: Vec<String> = env::args().collect();
    //println!("{:?}", args);

    let program = args[0].clone();

    let mut opts = Options::new();

    opts.optflag("h", "help", "print this help menu");
    opts.optflag("d", "detach", "detach process to run in background");
    opts.optopt("c", "count", "repeat the signal [count] times (default 1)", "INT");
    opts.optopt("t", "time", "send the signal in TIME seconds", "INT");
    opts.optopt("s", "signal", "signal to send (default=SIGINT)", "INT");
    //opts.optopt("p", "pid", "pid of the process to signal", "PID");

    let matches = match opts.parse(&args[1..]) {
        Ok(m) => { m }
        Err(f) => { panic!(f.to_string()) }
    };
    if matches.opt_present("h") {
        //println!("{} help called",program);
        print_usage(&program, &opts);
        return;
    }

    let detach = matches.opt_present("d");

    let count: u64 = match matches.opt_get_default("c",1) {
        Ok(m) => { m }
        Err(f) => error(&program, &opts, f.to_string())
    };
    let time: u64 = match matches.opt_get_default("t",0) {
        Ok(m) => { m }
        Err(f) => error(&program, &opts, f.to_string())
    };
    let mysignal: i32 = match matches.opt_get_default("s",15) {
        Ok(m) => { m }
        Err(f) => errori32(&program, &opts, f.to_string())
    };
    if matches.free.is_empty() {
        error(&program, &opts, "No PID supplied".to_string());
        println!("{} help called",program);
        return;
    };

    println!("detach={}",detach);
    println!("count={}",count);
    println!("time={}",time);
    println!("mysignal={}",mysignal);
    let signal = match signal::Signal::from_c_int(mysignal) {
        Ok(m) => { m }
        Err(f) => { panic!(f.to_string()) }
    };
    println!("{}",matches.free.len());
    for i in matches.free {
        println!("forking for {}",i);
        match fork() {
            Ok(ForkResult::Parent { child, .. }) => {
                println!("Continuing execution in parent process, new child has pid: {}", child);
            }
            Ok(ForkResult::Child) => {send_signal(time, i, signal, count)},
            Err(_) => println!("Fork failed"),
        }
    }
    process::exit(0)
}


fn send_signal(delay: u64, pidstr: String, signal: signal::Signal, count: u64){
    let pid: i32 = match pidstr.parse() {
        Ok(m) => { m }
        Err(f) => { panic!("error converting {} to pid: {}", pidstr, f)}
    };
    let p = unistd::Pid::from_raw(pid);
    let duration = time::Duration::from_secs(delay);

    for _i in 1..count {
        thread::sleep(duration);
    
        signal::kill(p, signal).expect("unable to kill ");
    }
    process::exit(0)
}

