use serde::Deserialize;
use std::fs;
use toml::from_str;

#[derive(Deserialize, Debug)]
pub struct Config {
    pub cpu: Option<Cpu>,
    pub temperature: Option<Temperature>,
}

#[derive(Deserialize, Debug)]
struct Cpu {
    time: Option<u32>,
}

#[derive(Deserialize, Debug)]
struct Temperature {
    time: Option<u32>,
}

pub fn get_configuration() -> Config {
    let toml = fs::read_to_string("~/.config/xtatusbar/config.toml")
        .map_or_else(|_| fs::read_to_string("config.toml"), Ok)
        .expect("Error reading file");

    let config: Config = from_str(&toml).expect("Error parsing TOML");
    config
//     println!("{:?}", parsed_toml);
}
