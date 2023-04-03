import { Injectable } from '@angular/core';
import { ParameterService } from './parameter.service';
import { ParameterGroup, Parameters } from '../utils/interfaces';

@Injectable({
  providedIn: 'root',
})
export class ParameterDragService {
  readonly format: string = 'applicaton/json';

  constructor(private parameterService: ParameterService) {}

  onDragStart(event: DragEvent, data: ParameterDragService.DragData) {
    event.dataTransfer?.setData(this.format, JSON.stringify(data));
  }

  dropInTrash(event: DragEvent) {
    event.preventDefault();
    if (event.dataTransfer) {
      let data: ParameterDragService.DragData = JSON.parse(
        event.dataTransfer.getData(this.format)
      );
      if (data.srcType === undefined || data.srcUUID === undefined) {
        return;
      }
      if (data.type === ParameterDragService.DataType.Group) {
        if (data.srcType === ParameterDragService.SrcType.Group) {
          this.parameterService.removeParamGroup(data.value);
        } else if (data.srcType === ParameterDragService.SrcType.Set) {
          this.parameterService
            .getParamSet(data.srcUUID)
            ?.removeGroup(data.value);
        }
      } else if (data.type === ParameterDragService.DataType.Var) {
        if (data.srcType === ParameterDragService.SrcType.Group) {
          this.parameterService
            .getParamGroup(data.srcUUID)
            ?.removeParam(data.value);
        } else if (data.srcType === ParameterDragService.SrcType.Set) {
          this.parameterService
            .getParamSet(data.srcUUID)
            ?.removeParam(data.value);
        }
      }
    }
  }

  drop(event: DragEvent, target: ParameterGroup | Parameters) {
    event.preventDefault();
    if (event.dataTransfer) {
      let data: ParameterDragService.DragData = JSON.parse(
        event.dataTransfer.getData(this.format)
      );
      if (
        data.type === ParameterDragService.DataType.Group &&
        target instanceof Parameters
      ) {
        target.addGroup(data.value);
      } else if (data.type === ParameterDragService.DataType.Var) {
        target.addParam(data.value);
      }
    }
  }

  onDragOver(event: DragEvent) {
    event.stopPropagation();
    event.preventDefault();
  }
}

export namespace ParameterDragService {
  export enum DataType {
    Var = 0,
    Group = 1,
    Set = 2,
    Callback = 3,
  }

  export enum SrcType {
    Group = 0,
    Set = 1,
  }

  export interface DragData {
    type: DataType;
    value: string;
    // For trashing
    srcType?: SrcType;
    srcUUID?: string;
  }
}
