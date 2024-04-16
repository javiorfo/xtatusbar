use std::fmt::Display;
use std::sync::{Arc, Mutex};
use std::thread;
use std::time::Duration;

use sysinfo::Components;
mod config;

fn main() {
    let result1_cache = Arc::new(Mutex::new(String::new()));
    let result2_cache = Arc::new(Mutex::new(String::new()));

    process(result1_cache.clone(), Duration::from_secs(1), || Widget {
        name: Some("nada".to_string()),
        icon: None,
        value: "30".to_string(),
    });

    process(result2_cache.clone(), Duration::from_millis(500), || {
        temperature()
    });
    principal(vec![result1_cache, result2_cache]);
}

fn process<F>(cache: Arc<Mutex<String>>, duration: Duration, fun: F)
where
    F: Fn() -> Widget + Send + 'static,
{
    thread::spawn(move || loop {
        let result = format!("{}", fun());
        *cache.clone().lock().unwrap() = result;
        thread::sleep(duration);
    });
}

fn principal(list: Vec<Arc<Mutex<String>>>) {
    loop {
        let mut bar = String::new();
        for value in list.iter() {
            let result = value.lock().unwrap().clone();
            bar.push_str(&result);
        }
        println!("{}", bar);

        thread::sleep(Duration::from_millis(100));
    }
}

fn temperature() -> Widget {
    let components = Components::new_with_refreshed_list();
    let total = components.iter().map(|c| c.temperature()).sum::<f32>();
    let total = total as usize / components.len();
    let total = format!("{}󰔄 ", total);
    Widget {
        name: Some("TEMP".to_string()),
        icon: Some("󰏈 ".to_string()),
        value: total,
    }
    //     format!("󰏈  TEMP {}󰔄 ", &total.to_string())
}

struct Widget {
    icon: Option<String>,
    name: Option<String>,
    value: String,
}

impl Display for Widget {
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
        let icon = self.icon.as_deref().unwrap_or("");
        let name = self.name.as_deref().unwrap_or("");
        write!(f, " {} {} {} ", icon, name, self.value)
    }
}

#[cfg(test)]
mod tests {
    use crate::config::configuration::get_configuration;

    //     use super::temperature;
    //
    //     #[test]
    //     fn testeo() {
    //         let content =
    //             temperature("/sys/devices/platform/coretemp.0/hwmon/hwmon1/temp1_input".as_ref());
    //         println!("{content}");
    //         println!("{}", content.len());
    //     }
    #[test]
    fn test_configuration() {
        get_configuration();
    }
}
