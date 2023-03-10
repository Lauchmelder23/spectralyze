use std::ffi::OsStr;

use clap::{builder::TypedValueParser, error::ErrorKind};

#[derive(Clone)]
pub struct TupleParser;

impl TypedValueParser for TupleParser {
    type Value = (u64, u64);
    
    fn parse_ref(
            &self,
            cmd: &clap::Command,
            arg: Option<&clap::Arg>,
            value: &std::ffi::OsStr,
        ) -> Result<Self::Value, clap::Error> {
        let inner = clap::value_parser!(u64);
        let raw = value.to_str().unwrap();

        let parts = raw.split(':').collect::<Vec<&str>>();
        
        if parts.len() < 2 {
            return Err(clap::Error::new(ErrorKind::TooFewValues));
        } 

        if parts.len() > 2 {
            return Err(clap::Error::new(ErrorKind::TooManyValues));
        }

        Ok((inner.parse_ref(cmd, arg, OsStr::new(parts[0]))?, inner.parse_ref(cmd, arg, OsStr::new(parts[1]))?))
    }
}