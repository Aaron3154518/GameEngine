import { parse, stringify } from 'uuid';
import { UUID, newUUID } from './utils';

// Parameter Group
export interface IParameterGroup {
  uuid: UUID;
  name: string;
  params: Set<string>;
}

type IParameterGroupOpt = {
  uuid?: UUID;
  name?: string;
  params?: Set<string>;
};

export class ParameterGroup implements IParameterGroup {
  readonly uuid: UUID;
  readonly name: string;
  params: Set<string>;

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
  }

  removeParam(param: string) {
    this.params.delete(param);
  }
}

// Parameters
export interface IParameters {
  uuid: UUID;
  name: string;
  type: string;
  groups: Set<UUID>;
}

interface IParametersOpt {
  uuid?: UUID;
  name?: string;
  type?: string;
  groups?: Set<UUID>;
}

export class Parameters implements IParameters {
  readonly uuid: UUID;
  readonly name: string;
  readonly type: string;
  group: ParameterGroup;
  groups: Set<UUID>;

  constructor({
    uuid = newUUID(),
    name = '',
    type = '',
    groups = new Set<UUID>(),
  }: IParametersOpt = {}) {
    this.uuid = uuid;
    this.name = name;
    this.type = type;
    this.group = new ParameterGroup();
    this.groups = groups;
    this.groups.add(this.group.uuid);
  }

  addParam(param: string) {
    this.group.addParam(param);
  }

  removeParam(param: string) {
    this.group.removeParam(param);
  }

  removeGroupParam(group: ParameterGroup, name: string) {
    this.removeGroup(group.uuid);
    group.params.forEach((p: string) => {
      if (p !== name) this.group.addParam(p);
    });
  }

  addGroup(id: UUID) {
    this.groups.add(id);
  }

  removeGroup(id: UUID) {
    if (id != this.group.uuid) {
      this.groups.delete(id);
    }
  }
}

// Callbacks
type CallbackParameters = { [key: string]: Set<string> };

export interface ICallback {
  uuid: UUID;
  name: string;
  code: string;
  // Parameters uuid (as string) : set of parameters names
  params: CallbackParameters;
}

interface ICallbackOpt {
  uuid?: UUID;
  name?: string;
  code?: string;
  // Parameters uuid (as string) : set of parameters names
  params?: CallbackParameters;
}

export class Callback implements ICallback {
  readonly uuid: UUID;
  readonly name: string;
  code: string;
  params: CallbackParameters;

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

  addParameter(uuid: UUID, name: string) {
    let id: string = stringify(uuid);
    let params: Set<string> = this.params[id];
    if (!params) {
      params = this.params[id] = new Set<string>();
    }
    params.add(name);
  }

  removeParameter(uuid: UUID, name: string) {
    this.params[stringify(uuid)]?.delete(name);
  }
}
