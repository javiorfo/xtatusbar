use std::path::Path;
use std::sync::{Arc, Mutex};
use std::time::Duration;
use std::{fs, thread};

use sysinfo::Components;

fn main() {
    let result1_cache = Arc::new(Mutex::new(String::new()));
    let result2_cache = Arc::new(Mutex::new(String::new()));

    process(result1_cache.clone(), Duration::from_secs(1), || {
        "nada".to_string()
    });

    process(result2_cache.clone(), Duration::from_millis(500), || {
        temperature("/sys/devices/platform/coretemp.0/hwmon/hwmon1/temp1_input".as_ref())
    });
    principal(vec![result1_cache, result2_cache]);
}

fn process<F>(cache: Arc<Mutex<String>>, duration: Duration, fun: F)
where
    F: Fn() -> String + Send + 'static,
{
    thread::spawn(move || {
        loop {
            let result = format!("Result from thread: {}", fun());
            *cache.clone().lock().unwrap() = result;
            thread::sleep(duration);
        }
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

fn temperature(_file_path: &Path) -> String {
//     let temp: u32 = fs::read_to_string(file_path)
//         .expect("File no read")
//         .trim()
//         .parse()
//         .unwrap();
//     let result = temp / 1000;
//     format!("󰏈  TEMP {}󰔄 ", &result.to_string())
    let components = Components::new_with_refreshed_list();
    let total = components.iter().map(|c| c.temperature()).sum::<f32>();
    let total = total as usize / components.len();
    format!("󰏈  TEMP {}󰔄 ", &total.to_string())
//     let mut result = String::new();
//     let result = components.iter().find(|c| c.label().contains("Package id 0")).map(|c| c.temperature().to_string()).unwrap();
//     for component in &components {
//         let formato = format!(" {}: {} ", component.label(), component.temperature());
//         result.push_str(&formato);
//     }
//     result
}

#[cfg(test)]
mod tests {
//     use super::temperature;
// 
//     #[test]
//     fn testeo() {
//         let content =
//             temperature("/sys/devices/platform/coretemp.0/hwmon/hwmon1/temp1_input".as_ref());
//         println!("{content}");
//         println!("{}", content.len());
//     }
}
