import { Observable, Subject } from 'rxjs';
import { newUUID } from './utils';

export type StringDict<T> = { [key: string]: T };

export function toDict<T>(list: T[], getKey: (t: T) => string): StringDict<T> {
  return list.reduce((dict: StringDict<T>, t: T) => {
    dict[getKey(t)] = t;
    return dict;
  }, {});
}

// Parameter Group
export interface IParameterGroup {
  uuid: string;
  name: string;
  params: Set<string>;
}

type IParameterGroupOpt = {
  uuid?: string;
  name?: string;
  params?: Set<string>;
};

export class ParameterGroup implements IParameterGroup {
  readonly uuid: string;
  readonly name: string;
  params: Set<string>;

  private changeSubject: Subject<void> = new Subject();
  $changes: Observable<void> = this.changeSubject.asObservable();

  constructor({
    uuid = newUUID(),
    name = '',
    params = new Set<string>(),
  }: IParameterGroupOpt = {}) {
    this.uuid = uuid;
    this.name = name;
    this.params = params;
  }

  hasParam(param: string): boolean {
    return this.params.has(param);
  }

  addParam(param: string) {
    this.params.add(param);
    this.changeSubject.next();
  }

  removeParam(param: string) {
    this.params.delete(param);
    this.changeSubject.next();
  }
}

// Parameters
export interface IParameters {
  uuid: string;
  name: string;
  type: string;
  groups: Set<string>;
}

type IParametersOpt = {
  uuid?: string;
  name?: string;
  type?: string;
  groups?: Set<string> | string[];
};

export class Parameters implements IParameters {
  readonly uuid: string;
  name: string;
  type: string;
  group: ParameterGroup;
  groups: Set<string>;

  private changeSubject: Subject<void> = new Subject();
  $changes: Observable<void> = this.changeSubject.asObservable();

  constructor({
    uuid = newUUID(),
    name = '',
    type = '',
    groups = [],
  }: IParametersOpt = {}) {
    this.uuid = uuid;
    this.name = name;
    this.type = type;
    this.group = new ParameterGroup();
    this.groups = new Set<string>(groups);

    let al = 'ABCDEFGHIJ';
    [...Array(al.length).keys()].forEach((i: number) =>
      this.group.addParam(al.substring(0, i + 1))
    );
  }

  addParam(param: string) {
    this.group.addParam(param);
    this.changeSubject.next();
  }

  removeParam(param: string) {
    this.group.removeParam(param);
    this.changeSubject.next();
  }

  addGroup(id: string): void {
    this.groups.add(id);
    this.changeSubject.next();
  }

  removeGroup(id: string): void {
    this.groups.delete(id);
    this.changeSubject.next();
  }
}

// Callbacks
export type CallbackParameters = StringDict<Set<string>>;

export interface ICallback {
  uuid: string;
  name: string;
  code: string;
  // Parameters uuid (as string) : set of parameters names
  params: CallbackParameters;
}

type ICallbackOpt = {
  uuid?: string;
  name?: string;
  code?: string;
  // Parameters uuid (as string) : set of parameters names
  params?: CallbackParameters;
};

export enum CodeType {
  Code = 0,
  Type = 1,
  Var = 2,
}

export class Callback implements ICallback {
  readonly uuid: string;
  name: string;
  code: string;
  params: CallbackParameters;

  private changeSubject: Subject<void> = new Subject();
  $changes: Observable<void> = this.changeSubject.asObservable();

  constructor({
    uuid = newUUID(),
    name = '',
    code = '',
    params = {},
  }: ICallbackOpt = {}) {
    this.uuid = uuid;
    this.name = name;
    this.code = code;
    this.params = params;
  }

  get signature(): string {
    return `void onUpdate(${Object.entries(this.params)
      .map(([id, params]: [string, Set<string>]) =>
        Object.values(params)
          .map((p: string) => `Type ${p}`)
          .join(',')
      )
      .join(', ')})`;
  }

  getSignatureSplit(parameters: StringDict<Parameters>): [string, CodeType][] {
    let first: boolean = true;
    return [
      ['void onUpdate(', CodeType.Code],
      ...Object.entries(this.params).reduce(
        (arr: [string, CodeType][], [id, params]: [string, Set<string>]) => {
          params.forEach((p: string) => {
            if (!first) {
              arr.push([', ', CodeType.Code]);
            } else {
              first = false;
            }
            arr.push([`${parameters[id].type} `, CodeType.Type]);
            arr.push([p, CodeType.Var]);
          });
          return arr;
        },
        []
      ),
      [') {', CodeType.Code],
    ];
  }

  addParameter(uuid: string, name: string) {
    let params: Set<string> = this.params[uuid];
    if (!params) {
      params = this.params[uuid] = new Set<string>();
    }
    params.add(name);
    this.changeSubject.next();
  }

  removeParameter(uuid: string, name: string) {
    this.params[uuid]?.delete(name);
    this.changeSubject.next();
  }

  static getParametersFromList(data: [string, string[]][]): CallbackParameters {
    return data.reduce(
      (params: CallbackParameters, [uuid, names]: [string, string[]]) => {
        params[uuid] = new Set<string>(names);
        return params;
      },
      {}
    );
  }
}
